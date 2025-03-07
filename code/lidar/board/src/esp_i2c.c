#include "esp_i2c.h"
#include "lidar.h"
#include "collision_handler.h"

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
static QueueHandle_t i2c_receive_queue, i2c_request_queue, i2c_write_queue;
i2c_slave_rx_done_event_data_t rx_data;

typedef struct {
    uint8_t *data;
    size_t length;
} i2c_write_data_t;

static bool i2c_slave_request_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_request_event_data_t *evt_data, void *arg)
{
    uint8_t length = 0;
    i2c_write_data_t write_data = {
        .data = (uint8_t *) malloc(I2C_BUFFER_SIZE * sizeof(uint8_t)),
        .length = 0
    };
    uint32_t actual_written_len = 0;
    if(xQueueReceive(i2c_write_queue, &write_data, 0) == pdTRUE) {
        ESP_ERROR_CHECK(i2c_slave_write(
            slave_handle, write_data.data, write_data.length, &actual_written_len, 100));  
            length = (uint8_t) actual_written_len;  
    }
    else {
        length = 250; 
    }

    BaseType_t xTaskWoken = 0;
    xQueueSendFromISR(i2c_request_queue, &length, &xTaskWoken);
    return xTaskWoken;
}

static bool i2c_slave_receive_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg)
{
    BaseType_t xTaskWoken = 0;
    xQueueSendFromISR(i2c_receive_queue, evt_data, &xTaskWoken);
    return xTaskWoken;
}

// Initialize the I2C slave
void i2c_slave_task(void *pvParameters) {
    ESP_LOGI("I2C 987654321", "beg I2C slave init");
    i2c_receive_queue = xQueueCreate(5, sizeof(i2c_slave_rx_done_event_data_t));
    i2c_request_queue = xQueueCreate(1, sizeof(i2c_slave_request_event_data_t));
    i2c_write_queue = xQueueCreate(1, sizeof(i2c_write_data_t));
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
    ESP_LOGI("I2C", "987654321 test");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    i2c_slave_event_callbacks_t cbs = {
        .on_receive = i2c_slave_receive_cb,
        .on_request = i2c_slave_request_cb
    };
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &cbs, NULL));

    i2c_write_data_t write_data = {
        .data = (uint8_t *) malloc(I2C_BUFFER_SIZE * sizeof(uint8_t)),
        .length = 0
    };

    ESP_LOGI("I2C", "init done");
    while (true) {
        i2c_slave_rx_done_event_data_t data;
        if (xQueueReceive(i2c_receive_queue, &data, 10) == pdTRUE) {
            if(data.length < 1) {
                ESP_LOGE("I2C", "Invalid data length < 1");
                continue;
            }
            ESP_LOGI("I2C", "Received %"PRIu32 "bytes", data.length);
            switch (data.buffer[0])
            {
            case I2C_REG_IS_OK:
                ESP_LOGI("I2C", "asking the lidar");
                write_data.data[0] =  is_lidar_running();
                write_data.length = 1;
                xQueueOverwrite(i2c_write_queue, &write_data);
                break;
            case I2C_REG_CONE:
                update_cone(data.buffer[1], data.buffer[2]);
                break;
            case I2C_REG_BOOL_OBSTACLE:
                write_data.data[0] = (uint8_t) has_obstacle() + 2;
                xQueueOverwrite(i2c_write_queue, &write_data);
                break;
            case I2C_REG_MM_OBSTACLE:
                write_data.data[0] = (uint8_t) closest_obstacle_dist();
                write_data.data[1] = (uint8_t) (closest_obstacle_dist() >> 8);
                xQueueOverwrite(i2c_write_queue, &write_data);
                break;
            default:
                ESP_LOGE("I2C", "Invalid register %"PRIu8, data.buffer[0]);
                break;
            }
        }
    }
    vTaskDelete(NULL);
}

void i2c_request_task(void *pvParameters) {
    uint8_t data;
    while(true) {
        if(xQueueReceive(i2c_request_queue, &data, 100 /portTICK_PERIOD_MS) == pdTRUE) {
            ESP_LOGI("I2C", "request error code : %i", data);
        }

    }
    /* 
    i2c_slave_rx_done_event_data_t data;
    i2c_write_data_t write_data = {
        .data = (uint8_t *) malloc(I2C_BUFFER_SIZE * sizeof(uint8_t)),
        .length = 0
    };
    uint32_t actual_written_len = 0;
    uint8_t *send_buffer = (uint8_t *) malloc(I2C_BUFFER_SIZE * sizeof(uint8_t));
    while (true) {
        if(xQueueReceive(i2c_request_queue, &data, 10) == pdTRUE) {
            if(xQueueReceive(i2c_write_queue, &write_data, 10) == pdTRUE) {
            ESP_LOGI("I", "length %i", write_data.length);
            ESP_LOGI("I2C_request", "to write first pos %i, len %i", write_data.data[0], write_data.length);
            ESP_ERROR_CHECK(i2c_slave_write(
                slave_handle, write_data.data, write_data.length, &actual_written_len, 100));
        
            if(actual_written_len == 0 ||actual_written_len != write_data.length) {
                ESP_LOGI("I2C", "Written unexpected amount of data: %"PRIu32 "bytes", actual_written_len);
            }
            
        }   
    }
}*/}