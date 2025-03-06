#include "i2c.h"
#include "driver/i2c_slave.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "string.h"

// Define the I2C port number
#define I2C_PORT_NUM I2C_NUM_0

// Define the I2C pins
#define I2C_SLAVE_SDA_IO 21
#define I2C_SLAVE_SCL_IO 22

// Define the I2C buffer size
#define I2C_SLAVE_BUF_LEN 64

static uint8_t i2c_slave_buffer[I2C_SLAVE_BUF_LEN];
static i2c_slave_dev_handle_t slave_handle;
static QueueHandle_t i2c_slave_queue;
i2c_slave_rx_done_event_data_t rx_data;

typedef enum {
    I2C_SLAVE_EVT_RX,
    I2C_SLAVE_EVT_TX
} i2c_slave_event_t;


static bool i2c_slave_request_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_request_event_data_t *evt_data, void *arg)
{
    i2c_slave_rx_done_event_data_t *context = (i2c_slave_rx_done_event_data_t *)arg;
    i2c_slave_event_t evt = I2C_SLAVE_EVT_TX;
    BaseType_t xTaskWoken = 0;
    xQueueSendFromISR(context->event_queue, &evt, &xTaskWoken);
    return xTaskWoken;
}

static bool i2c_slave_receive_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg)
{
    i2c_slave_rx_done_event_data_t *context = (i2c_slave_rx_done_event_data_t *)arg;
    i2c_slave_event_t evt = I2C_SLAVE_EVT_RX;
    BaseType_t xTaskWoken = 0;
    context->buffer = *evt_data->buffer;
    context->length = evt_data->length;
    xQueueSendFromISR(context->event_queue, &evt, &xTaskWoken);
    return xTaskWoken;
}

// Initialize the I2C slave
void i2c_slave_task(void *pvParameters) {
    int i2c_slave_rx_buf_len = I2C_SLAVE_BUF_LEN;
    int i2c_slave_tx_buf_len = I2C_SLAVE_BUF_LEN;

    i2c_slave_queue = xQueueCreate(5, sizeof(i2c_slave_buffer));

    i2c_slave_config_t i2c_slv_config = {
        .i2c_port = I2C_PORT_NUM,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = I2C_SLAVE_ADDRESS,
        .send_buf_depth = I2C_SLAVE_BUF_LEN,
        .receive_buf_depth = I2C_SLAVE_BUF_LEN,
        .flags.enable_internal_pullup = false,
    };
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    i2c_slave_event_callbacks_t cbs = {
        .on_receive = i2c_slave_receive_cb,
        .on_request = i2c_slave_request_cb,
    };
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &cbs, &rx_data));

    uint8_t zero_buffer[32] = {}; // Use this buffer to clear the fifo.
    uint32_t write_len, total_written;
    uint32_t buffer_size = 0;

    while (true) {
        i2c_slave_event_t evt;
        if (xQueueReceive(i2c_slave_queue, &evt, 10) == pdTRUE) {
            if (evt == I2C_SLAVE_EVT_TX) {
                uint8_t *data_buffer;
                //TODO : traiter la commande !
                switch (context->command_data) {
                case STARS_COMMAND:
                    data_buffer = context->tmp_buffer_stars;
                    buffer_size = sizeof(context->tmp_buffer_stars);
                    break;
                case FORKS_COMMAND:
                    data_buffer = context->tmp_buffer_forks;
                    buffer_size = sizeof(context->tmp_buffer_forks);
                    break;
                case OPENISSUES_COMMAND:
                    data_buffer = context->tmp_buffer_open_issues;
                    buffer_size = sizeof(context->tmp_buffer_open_issues);
                    break;
                case DESCRIPTIONS_COMMAND:
                    data_buffer = context->tmp_buffer_descriptions;
                    buffer_size = sizeof(context->tmp_buffer_descriptions);
                    break;
                default:
                    ESP_LOGE(TAG, "Invalid command");
                    data_buffer = zero_buffer;
                    buffer_size = sizeof(zero_buffer);
                    break;
                }

                //TODO : WRITE DATA !
                total_written = 0;
                while (total_written < buffer_size) {
                    ESP_ERROR_CHECK(i2c_slave_write(handle, data_buffer + total_written, buffer_size - total_written, &write_len, 1000));
                    if (write_len == 0) {
                        ESP_LOGE(TAG, "Write error or timeout");
                        break;
                    }
                    total_written += write_len;
                }
            }
        }
    }
    vTaskDelete(NULL);
}

// Write data to the I2C slave
void i2c_slave_write(uint8_t *data, size_t length) {
    if (length > I2C_SLAVE_BUF_LEN) {
        ESP_LOGE("I2C_SLAVE", "Data too long");
        return;
    }
    memcpy(i2c_slave_buffer, data, length);
    i2c_slave_write_buffer(I2C_PORT_NUM, i2c_slave_buffer, length, 1000 / portTICK_PERIOD_MS);
}

// Read data from the I2C slave
void i2c_slave_read(uint8_t *data, size_t length) {
    if (length > I2C_SLAVE_BUF_LEN) {
        ESP_LOGE("I2C_SLAVE", "Data too long");
        return;
    }
    if(length == 0) {
        return;
    }
    esp_err_t err = i2c_slave_read_buffer(I2C_PORT_NUM, data, length, 1000 / portTICK_PERIOD_MS);
    if(err != ESP_OK) {
        ESP_LOGE("I2C_SLAVE", "Read failed: %s", esp_err_to_name(err));
        return;
    }
    ESP_LOGI("I2C_SLAVE", "Read data: %s", data);
}
