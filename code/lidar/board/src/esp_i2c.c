#include "esp_i2c.h"
#include "lidar.h"
#include "collision_handler.h"

#include "driver/i2c_slave.h"
#include "esp32-hal-i2c-slave.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "string.h"
#include <inttypes.h>

#define QUEUE_STRING(queue, fmt, ...) do {                            \
    char *stringToQueue = (char *)pvPortMalloc(32 * sizeof(char));    \
    snprintf(stringToQueue, 32, fmt, __VA_ARGS__);                    \
    xQueueSend((queue), &stringToQueue, portMAX_DELAY);               \
} while(0)

// Define the I2C port number
#define I2C_PORT_NUM I2C_NUM_0

// Define the I2C pins
#define I2C_SLAVE_SDA_IO 3
#define I2C_SLAVE_SCL_IO 4

static i2c_slave_dev_handle_t slave_handle;
static QueueHandle_t i2c_receive_queue, log_queue;
i2c_slave_rx_done_event_data_t rx_data;

uint8_t rx_buffer[32] = {0};
uint8_t tx_buffer[32] = {0};
uint8_t data_len = 0;

void request_cb(uint8_t num, void *arg) {
    if(tx_buffer[0] == 0) {
        ESP_LOGI("ESPI2C", "request not fast enough");
    }
    else {
        i2cSlaveWrite(num, tx_buffer, data_len, 100);
        data_len = 0;
    }
}

void receive_cb(uint8_t num, uint8_t *data, size_t len, bool stop, void *arg) {
    if(len != 1) {
        ESP_LOGW("ESPI2C", "receive len != 1");
    }
    ESP_LOGI("I2C", "data %i", data[0]);
    switch (data[0])
    {
    case I2C_REG_IS_OK:
    tx_buffer[0] = is_lidar_running() ? 5 : 4;
        data_len = 1;
        break;
    case I2C_REG_CONE:
        float unmap_center_angle = (data[1] / 256.0 * 2 * M_PI) - M_PI;
        float unmap_half_cone_width = data[2] / 256.0 * M_PI;
        update_cone(unmap_center_angle, unmap_half_cone_width);
        break;
    case I2C_REG_BOOL_OBSTACLE:
    tx_buffer[0] = (uint8_t) has_obstacle() + 4;
        data_len = 1;
        break;
    case I2C_REG_MM_OBSTACLE:
        uint16_t dist = closest_obstacle_dist();
        tx_buffer[0] = (uint8_t) dist;
        tx_buffer[1] = (uint8_t) (dist >> 8);
        data_len = 2;
        break;
    case I2C_REG_BUTTON5:
        tx_buffer[0] = (uint8_t) gpio_get_level(GPIO_NUM_5) + 4;
        data_len = 1;
        break;
    case I2C_REG_BUTTON7:
        tx_buffer[0] = (uint8_t) gpio_get_level(GPIO_NUM_7) + 4;
        data_len = 1;
        break;
    default:
        ESP_LOGE("I2C", "Invalid register %"PRIu8, data[0]);
        break;
    }
}

// Initialize the I2C slave
void i2c_slave_task(void *pvParameters) {
    ESP_LOGI("I2C 987654321", "beg I2C slave init");


    i2cSlaveAttachCallbacks(I2C_PORT_NUM, request_cb, receive_cb, NULL);
    if (i2cSlaveInit(I2C_PORT_NUM, I2C_SLAVE_SDA_IO, I2C_SLAVE_SCL_IO, I2C_SLAVE_ADDRESS, 100000, I2C_BUFFER_SIZE, I2C_BUFFER_SIZE) != ESP_OK) {
      ESP_LOGE("I2C", "Slave Init ERROR");
    }
    
    ESP_LOGI("I2C", "init done");


    while (true) {
        vTaskDelay(100/portTICK_PERIOD_MS);
        ESP_LOGI("I2C", "is_running");

    }
    vTaskDelete(NULL);
}

void printer_log_task(void *pvParameters) {
    char *log;
    for(;;) {
        while (uxQueueMessagesWaiting(log_queue) > 0) {
            if (xQueueReceive(log_queue, &log, portMAX_DELAY)) {
                //ESP_LOGI("PRINTER", "%s", log);
                vPortFree(log);
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
