#include "lidar.h"
#include "parser.h"

#include <driver/uart.h>
#include "driver/gpio.h"
#include <hal/uart_hal.h>
#include <esp_err.h>
#include <esp_log.h>
#include <rom/ets_sys.h>

//Temporary :: to be removed
#include "esp_heap_caps.h"
#include <sys/time.h>

#include "../loca_lidar/amalgame.h"
#include "../loca_lidar/loca_lidar.h"
#include "../loca_lidar/pose_refinement.h"




#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define UART_PORT UART_NUM_0
#define UART_TX_PIN 1
#define UART_RX_PIN 20
#define LIDAR_TASK_PRIORITY 5
#define TIMEOUT_CYCLE 5 //Reset the parser if not receiving lidar data for more than... Only reset in fullscan

#define TAG "LIDAR"

static QueueHandle_t uart0_queue;
static bool is_ok = false;
QueueHandle_t polar_array_queue;

amalgame_t* full_amalgames = NULL;
int nb_amalg = 0;

static void lidar_task(void *pvParameter);
void init_uart() {
    uart_config_t uart_config = {
        .baud_rate = 230400,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_PIN_NO_CHANGE, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, 1024, 0, 4, &uart0_queue, 0));
    xTaskCreate(lidar_task, "LidarTask", 4096, NULL, LIDAR_TASK_PRIORITY, NULL);
    polar_array_queue = xQueueCreate(1, sizeof(polar_array_t));

    //Prevent strange signal sending to lidar (until having a proper PWM)
    //gpio_config_t io_conf;
    //io_conf.intr_type = GPIO_INTR_DISABLE; // Disable interrupts
    //io_conf.mode = GPIO_MODE_INPUT;        // Set as input mode
    //io_conf.pin_bit_mask = (1ULL << GPIO_NUM_21); // Bit mask of the pin
    //io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;    // Disable pull-down
    //io_conf.pull_up_en = GPIO_PULLUP_DISABLE;        // Disable pull-up
    //gpio_config(&io_conf);   
}

static void lidar_task(void *pvParameter) {
    uint16_t timeout_wait = 0;
    int buffer_length = 0;
    int read_length = 0;
    int expected_length = 1;
    bool full_scan = false;
    bool high_buffer_usage = false;
    uint8_t data[128];
    pose_t estimated_odom = {
        .angle_rad = 0,
        .pos = {0.0, 0.0}
    };

    amalgame_t* full_amalgames = (amalgame_t*) calloc(amalgame_finder_tuning.max_amalg_count, sizeof(amalgame_t));
    polar_array_t polar_array;
    init_amalgames(amalgame_finder_tuning, full_amalgames);

    while (true)
    {
        //Lidar UART reading management
        ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT, (size_t*)&buffer_length));
        read_length = uart_read_bytes(UART_PORT, data, expected_length, 10);
        if(buffer_length <50) {
            vTaskDelay(10 / portTICK_PERIOD_MS); 
        }
        if(high_buffer_usage == false && buffer_length > 900) {
            ESP_LOGE(TAG, "UART buffer high usage %i", buffer_length);
            high_buffer_usage = true;
        }

        if(read_length == 0) {
            ESP_LOGE(TAG, "no UART data received");
            if(timeout_wait++ == TIMEOUT_CYCLE) {
                reset_parser();
            }
            if(timeout_wait > 16000) {
                timeout_wait = 0; //prevent overflow
            }
            vTaskDelay(10 / portTICK_PERIOD_MS); 
        }
        else if (read_length < 0) {
            ESP_LOGE(TAG, "error reading uart : %i", read_length);
            vTaskDelay(10 / portTICK_PERIOD_MS); 
        }
        else if (read_length < expected_length) {
            ESP_LOGE(TAG, "date length from lidar problem");
            vTaskDelay(10 / portTICK_PERIOD_MS); 
        }
        expected_length = parse_chunk(data, &full_scan);

        //lidar fullscan processing
        if (full_scan)
        {
            is_ok = true;
            timeout_wait = 0;
            //generate raw lidar
            raw_lidar_t* out_lidar = (raw_lidar_t*) malloc(sizeof(raw_lidar_t));
            parse_frames(out_lidar);
            taskYIELD();
            //Generate amalgames
            nb_amalg = calc_amalgames(amalgame_finder_tuning, *out_lidar, full_amalgames);
            taskYIELD();
            if (nb_amalg >= amalgame_finder_tuning.max_amalg_count - 1)
            {
                ESP_LOGI("amalgame", "Max amalgame reached %i", nb_amalg);
            }
            
            copy_amalgames_avg(polar_array.pts, nb_amalg, full_amalgames);
            polar_array.length = nb_amalg;
            xQueueOverwrite(polar_array_queue, &polar_array);
            taskYIELD();
            //Convert to cartesian
            point_t* pts = (point_t*) malloc(nb_amalg * sizeof(point_t));
            uint16_t* avg_angles = (uint16_t*) malloc(nb_amalg * sizeof(uint16_t));
            uint16_t* avg_dists = (uint16_t*) malloc(nb_amalg * sizeof(uint16_t));
            for (size_t i = 0; i < nb_amalg; i++)
            {
                avg_angles[i] = full_amalgames[i].avg_angle;
                avg_dists[i] = full_amalgames[i].avg_dist;
            }
            convert_xy(pts, nb_amalg, avg_angles, avg_dists);
            taskYIELD();
            //Calculate pose
            pose_t pose = refine_pose(pts, full_amalgames, nb_amalg, estimated_odom, &pose_tuning);
            taskYIELD();
            free((void*)avg_angles);
            free((void*)avg_dists);
            free((void*) pts);
            clean_amalgames(amalgame_finder_tuning, full_amalgames, nb_amalg);
            free((void*) out_lidar->angles);
            free((void*) out_lidar->distances);
            free((void*) out_lidar->intensities);
            free((void*) out_lidar);
            //ESP_LOGI(TAG, "Full scan processed");
            taskYIELD();
        }       
    }
    

}

bool is_lidar_running() {
    return is_ok;
}