#include "i2c_slave.h"
#include "esp_log.h"

// Define the I2C port number
#define I2C_PORT_NUM I2C_NUM_0

// Define the I2C pins
#define I2C_SLAVE_SDA_IO 21
#define I2C_SLAVE_SCL_IO 22

// Define the I2C buffer size
#define I2C_SLAVE_BUF_LEN 64

// Static variables
static uint8_t i2c_slave_buffer[I2C_SLAVE_BUF_LEN];

// Initialize the I2C slave
void i2c_slave_init(void) {
    int i2c_slave_rx_buf_len = I2C_SLAVE_BUF_LEN;
    int i2c_slave_tx_buf_len = I2C_SLAVE_BUF_LEN;

    // Configure the I2C slave
    i2c_config_t conf = {
        .mode = I2C_MODE_SLAVE,
        .slave = {
            .addr_10bit_en = 0,
            .slave_addr = I2C_SLAVE_ADDRESS,
        },
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .sda_pullup_en = GPIO_PULLDOWN_DISABLE,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .scl_pullup_en = GPIO_PULLDOWN_DISABLE,
        .master.clk_speed = 100000,
    };

    // Install the I2C driver
    i2c_param_config(I2C_PORT_NUM, &conf);
    i2c_driver_install(I2C_PORT_NUM, conf.mode, i2c_slave_rx_buf_len, i2c_slave_tx_buf_len, 0);
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
    i2c_slave_read_buffer(I2C_PORT_NUM, data, length, 1000 / portTICK_PERIOD_MS);
    ESP_LOGI("I2C_SLAVE", "Read data: %s", data);
}
