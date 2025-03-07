#include "esp_i2c.h"
#include "driver/i2c.h"

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

static esp_err_t i2c_slave_init(void)
{
    int i2c_slave_port = I2C_SLAVE_ADDRESS;
    i2c_config_t conf_slave = {
        .sda_io_num = I2C_PORT_NUM,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .mode = I2C_MODE_SLAVE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = I2C_SLAVE_SDA_IO,
    };
    esp_err_t err = i2c_param_config(i2c_slave_port, &conf_slave);
    if (err != ESP_OK) {
        return err;
    }
    return i2c_driver_install(i2c_slave_port, conf_slave.mode, 64, 64, 0);
}

// Initialize the I2C slave
void i2c_slave_task(void *pvParameters) {
    ESP_ERR_CHECK(i2c_slave_init());
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP_LOGI("I2C", "I2C slave initialized");
    while (true) {
        // Read data from the I2C slave
        int data = 0;
        i2c_slave_read_buffer(I2C_SLAVE_ADDRESS, &data, 1, 1000 / portTICK_PERIOD_MS);
        ESP_LOGI("I2C", "Received data: %d", data);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

    }
}
