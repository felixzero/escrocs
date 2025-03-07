#include "esp_i2c.h"
#include "driver/i2c_slave.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "string.h"
#include <inttypes.h>

// Define the I2C port number
#define I2C_PORT_NUM I2C_NUM_0

// Define the I2C pins
#define I2C_SLAVE_SDA_IO 3
#define I2C_SLAVE_SCL_IO 4

static i2c_slave_dev_handle_t slave_handle;
static QueueHandle_t i2c_slave_queue;
i2c_slave_rx_done_event_data_t rx_data;

typedef enum {
    I2C_SLAVE_EVT_RX,
    I2C_SLAVE_EVT_TX
} i2c_slave_event_t;

static IRAM_ATTR bool i2c_slave_receive_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg)
{
    i2c_slave_rx_done_event_data_t *context = (i2c_slave_rx_done_event_data_t *)arg;
    i2c_slave_event_t evt = I2C_SLAVE_EVT_RX;
    BaseType_t xTaskWoken = 0;
    context->buffer = (uint8_t *) malloc(evt_data->length);
    memcpy(context->buffer, evt_data->buffer, evt_data->length);
    context->length = evt_data->length;
    xQueueSendFromISR(i2c_slave_queue, &evt, &xTaskWoken);
    return xTaskWoken;
}

// Initialize the I2C slave
void i2c_slave_task(void *pvParameters) {
    i2c_slave_queue = xQueueCreate(5, sizeof(i2c_slave_rx_done_event_data_t));
    rx_data.buffer = (uint8_t *)malloc(I2C_BUFFER_SIZE);

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .i2c_port = I2C_PORT_NUM,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = I2C_SLAVE_ADDRESS,
        .send_buf_depth = I2C_BUFFER_SIZE,
        .receive_buf_depth = I2C_BUFFER_SIZE,
        .flags.enable_internal_pullup = false,
    };
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    i2c_slave_event_callbacks_t cbs = {
        .on_receive = i2c_slave_receive_cb,
    };
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &cbs, &rx_data));

    uint8_t zero_buffer[32] = {}; // Use this buffer to clear the fifo.
    uint32_t write_len, total_written;
    uint32_t buffer_size = 0;
    while (true) {
        i2c_slave_rx_done_event_data_t data;
        data.buffer = (uint8_t *)malloc(I2C_BUFFER_SIZE);
        if (xQueueReceive(i2c_slave_queue, &data, 10) == pdTRUE) {
            if(data.length < 1) {
                ESP_LOGE("I2C", "Invalid data length < 1");
                continue;
            }
            ESP_LOGI("I2C", "Received %"PRIu32 "bytes", data.length);
            ESP_LOGI("I2C", "data %i", data.buffer[0]);
            switch (data.buffer[0])
            {
            case I2C_REG_IS_OK:
                //ESP_LOGI("I2C", "Received I2C_REG_IS_OK");
                break;
            case I2C_REG_CONE:
               /* code */
                break;
            case I2C_REG_BOOL_OBSTACLE:
                /* code */
                break;
                case I2C_REG_MM_OBSTACLE:
                /* code */
                break;
            default:
                break;
            }
        }

                //i2c_slave_transmit(handle, data_buffer, buffer_size, 1000);
                //TODO : WRITE DATA !
               //total_written = 0;
               //while (total_written < buffer_size) {
               //    ESP_ERROR_CHECK(i2c_slave_write(handle, data_buffer + total_written, buffer_size - total_written, &write_len, 1000));
               //    if (write_len == 0) {
               //        ESP_LOGE(TAG, "Write error or timeout");
               //        break;
               //    }
               //    total_written += write_len;
               //}
    }
    vTaskDelete(NULL);
}
