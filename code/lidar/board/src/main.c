#include <driver/uart.h>
#include <hal/uart_hal.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <rom/ets_sys.h>

#include "parser.h"

#include "amalgame.h"
#include "loca_lidar.h"
#include "pose_refinement.h"

#include "esp_heap_caps.h"
#include <sys/time.h>

#define UART_PORT UART_NUM_0
#define CHUNK_LENGTH 47 //11 + 3 * 12
#define TAG "LIDAR"

QueueHandle_t uart0_queue;
void config_uart() {
    uart_config_t uart_config = {
        .baud_rate = 230400,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, 1, 20, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, 1024, 0, 4, &uart0_queue, 0));}

void app_main() {
    config_uart();
    int buffer_length = 0;
    int read_length = 0;
    int expected_length = 1;
    bool full_scan = false;
    bool high_buffer_usage = false;
    uint8_t data[128];

    amalgame_t* full_amalgames = (amalgame_t*) calloc(amalgame_finder_tuning.max_amalg_count, sizeof(amalgame_t));
    init_amalgames(amalgame_finder_tuning, full_amalgames);

    while (true)
    {
        //Lidar UART reading management
        ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT, (size_t*)&buffer_length));
        read_length = uart_read_bytes(UART_PORT, data, expected_length, 10);
        if(buffer_length <50) {
            ets_delay_us(10);   
        }
        if(high_buffer_usage == false && buffer_length > 900) {
            ESP_LOGE(TAG, "UART buffer high usage %i", buffer_length);
            high_buffer_usage = true;
        }

        if(read_length == 0) {
            ESP_LOGE(TAG, "no UART data received");
        }
        else if (read_length < 0) {
            ESP_LOGE(TAG, "error reading uart : %i", read_length);
        }
        else if (read_length < expected_length) {
            ESP_LOGE(TAG, "date length from lidar problem");
        }
        expected_length = parse_chunk(data, &full_scan);

        //lidar fullscan processing
        if (full_scan)
        {
            //generate raw lidar
            raw_lidar_t* out_lidar = (raw_lidar_t*) malloc(sizeof(raw_lidar_t));
            parse_frames(out_lidar);

            //Generate amalgames
            int nb_amalg = calc_amalgames(amalgame_finder_tuning, *out_lidar, full_amalgames);
            if (nb_amalg >= amalgame_finder_tuning.max_amalg_count - 1)
            {
                ESP_LOGI("amalgame", "Max amalgame reached %i", nb_amalg);
            }
            

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

            //Calculate pose
            pose_t pose = refine_pose(pts, full_amalgames, nb_amalg, &pose_tuning);

            free((void*)avg_angles);
            free((void*)avg_dists);
            free((void*) pts);
            clean_amalgames(amalgame_finder_tuning, full_amalgames, nb_amalg);
            free((void*) out_lidar->angles);
            free((void*) out_lidar->distances);
            free((void*) out_lidar->intensities);
            free((void*) out_lidar);
            ESP_LOGI(TAG, "Full scan processed");

        }       
    }
    

}