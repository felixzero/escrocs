#include <stdio.h>
#include <string.h>

#include "system/i2c_master.h"

#define I2C_SDA_PIN_MOTOR 18
#define I2C_SCL_PIN_MOTOR 19
#define I2C_PORT_MOTOR I2C_NUM_0
#define I2C_CLOCK_MOTOR 100000

#define I2C_SDA_PIN_PERIPH 16
#define I2C_SCL_PIN_PERIPH 17
#define I2C_PORT_PERIPH I2C_NUM_1
#define I2C_CLOCK_PERIPH 100000

#define I2C_TIMEOUT_MS 500
#define I2C_MAX_BUFFER_SIZE 64

void init_i2c_master(void)
{
    printf("Initializing I2C master...\n");

    i2c_config_t conf_motor = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN_MOTOR,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SCL_PIN_MOTOR,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_CLOCK_MOTOR,
    };
    i2c_param_config(I2C_PORT_MOTOR, &conf_motor);

    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT_MOTOR, conf_motor.mode, 0, 0, 0));
    i2c_set_timeout(I2C_PORT_MOTOR, 80000);

    i2c_config_t conf_perif = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN_PERIPH,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SCL_PIN_PERIPH,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_CLOCK_PERIPH,
    };
    i2c_param_config(I2C_PORT_PERIPH, &conf_perif);

    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT_PERIPH, conf_perif.mode, 0, 0, 0));
    i2c_set_timeout(I2C_PORT_PERIPH, 80000);
}

void send_to_i2c(int port, uint8_t slave_addr, void *buffer, size_t length)
{
    i2c_cmd_handle_t i2c_handle = i2c_cmd_link_create();
    i2c_master_start(i2c_handle);
    i2c_master_write_byte(i2c_handle, (slave_addr << 1) | I2C_MASTER_WRITE, 1);
    for (int i = 0; i < length; ++i) {
        i2c_master_write_byte(i2c_handle, ((uint8_t*)buffer)[i], 1);
    }
    i2c_master_stop(i2c_handle);

    // Send to I2C bus
    esp_err_t err = i2c_master_cmd_begin(port, i2c_handle, I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);
    if (err != ESP_OK) {
        i2c_reset_tx_fifo(port);
    }
    i2c_cmd_link_delete(i2c_handle);
}

void request_from_i2c(int port, uint8_t slave_addr, void *buffer, size_t length)
{
    i2c_cmd_handle_t i2c_handle = i2c_cmd_link_create();
    i2c_master_start(i2c_handle);
    i2c_master_write_byte(i2c_handle, (slave_addr << 1) | I2C_MASTER_READ, 0);

    for (int i = 0; i < length - 1; ++i) {
        i2c_master_read(i2c_handle, buffer + i, 1, 0);
    }
    i2c_master_read(i2c_handle, buffer + length - 1, 1, 1);
    i2c_master_stop(i2c_handle);

    esp_err_t err = i2c_master_cmd_begin(port, i2c_handle, I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);
    if (err != ESP_OK) {
        i2c_reset_rx_fifo(port);
    }
    i2c_cmd_link_delete(i2c_handle);
}

void write_i2c_register(int port, uint8_t slave_addr, uint8_t register_addr, uint8_t value)
{
    uint8_t buffer[2];
    buffer[0] = register_addr;
    buffer[1] = value;
    send_to_i2c(port, slave_addr, buffer, sizeof(buffer));
}

uint8_t read_i2c_register(int port, uint8_t slave_addr, uint8_t register_addr)
{
    uint8_t value;
    send_to_i2c(port, slave_addr, &register_addr, 1);
    request_from_i2c(port, slave_addr, &value, 1);
    return value;
}

