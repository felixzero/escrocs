#include <driver/uart.h>
#include <esp_log.h>

void app_main() {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
    };

    ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, UART_PIN_NO_CHANGE, 5, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;
    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));

    while (1) {
        uint8_t data[128];
        int length = uart_read_bytes(UART_NUM_2, data, 128, 20 / portTICK_RATE_MS);

        if (length > 0) {
            ESP_LOGI("CC", "%d", length);
            _BUFFER_HEXDUMP("HEX", data, length, ESP_LOG_INFO);
        }

        //vTaskDelay(1);
    }
}