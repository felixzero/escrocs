#include "ld06_driver.h"
#include <esp_err.h>
#include <esp_log.h>
#include <string.h>

#define MAX_PT_COUNT 12

const uart_port_t lidar_port = UART_NUM_1;

static int cur_length = 0;
static uint16_t expected_length = 1;
static Msg_type msg_type = START;
static uint16_t count = 0; //nb of points
static float total_angle = 0;
static uint8_t* data;

static void add_point();
static void get_angles(uint16_t* angles);
static void get_distances(uint16_t* distances);

void init_lidar(uint8_t rx_pin) {
    uart_config_t uart_config = {
        .baud_rate = 230400,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(uart_param_config(lidar_port, &uart_config));
    
    ESP_ERROR_CHECK_WITHOUT_ABORT(uart_set_pin(lidar_port, 21, 20, -1, -1));

    const int uart_buffer_size = (1024 * 2);
    ESP_ERROR_CHECK_WITHOUT_ABORT(uart_driver_install(lidar_port, uart_buffer_size, 0, 0, NULL, 0));
    
    data = (uint8_t*) malloc(9+3*MAX_PT_COUNT);
} 

void scan_lidar() {
    ESP_ERROR_CHECK_WITHOUT_ABORT(
        uart_get_buffered_data_len(lidar_port, (size_t*)&cur_length));
    if(cur_length > 500) {
        ESP_LOGI("driver", "length %i", cur_length);
    }
    if (cur_length < expected_length) {
        vTaskDelay(10/portTICK_PERIOD_MS);
        return;
    }

    memset(data, 0, sizeof(data));
    uart_read_bytes(lidar_port, data, expected_length, 10/ portTICK_PERIOD_MS);
    if(msg_type == START) {
        if(data[0] == 0x54) {
            msg_type = LEN;
            expected_length = 1;
        }
    }
    elif (msg_type == LEN) {
        count = data[0] & 0x0E;
        msg_type = DATA;
        expected_length = 9 + 3 * count;
    }
    elif (msg_type == DATA) {
        speed = data[0:2];
    }

}