#include <driver/uart.h>
#include <hal/uart_hal.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <rom/ets_sys.h>

#include "parser.h"

#include <sys/time.h> // for gettimeofday

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
    while (true)
    {
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


        

        if (full_scan)
        {
            raw_lidar_t* out_lidar = (raw_lidar_t*) malloc(sizeof(raw_lidar_t));
            parse_frames(out_lidar);
            free(out_lidar->angles);
            free(out_lidar->distances);
            free(out_lidar->intensities);
            free(out_lidar);
            ESP_LOGI(TAG, "Full scan processed");
  
            

        }       
    }
    

}