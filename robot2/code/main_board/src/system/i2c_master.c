#include <stdio.h>
#include <string.h>
#include <driver/i2c.h>

#include "system/i2c_master.h"

#define I2C_SDA_PIN 19
#define I2C_SCL_PIN 21
#define I2C_CLOCK 10000
#define I2C_PORT I2C_NUM_0
#define I2C_TIMEOUT_MS 10
#define I2C_MAX_BUFFER_SIZE 32

void init_i2c_master(void)
{
    printf("Initializing I2C master...\n");

    // Set I2C as master
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_io_num = I2C_SCL_PIN,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = I2C_CLOCK,
    };
    i2c_param_config(I2C_PORT, &conf);

    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0));
    i2c_set_timeout(I2C_NUM_0, 80000);
}

void send_i2c_command(uint8_t slave_addr, const char *command_format, ...)
{
    va_list parameters;
    va_start(parameters, command_format);

    // Build I2C command sequence from format string
    i2c_cmd_handle_t i2c_handle = i2c_cmd_link_create();
    i2c_master_start(i2c_handle);
    i2c_master_write_byte(i2c_handle, (slave_addr << 1) | I2C_MASTER_WRITE, 1);

#define _handle_case(data_type, key, true_type) { \
    if (command_format[0] == key) { \
        data_type value = va_arg(parameters, true_type); \
        memcpy(buffer + buffer_index, (uint8_t*)&value, sizeof(data_type)); \
        buffer_index += sizeof(data_type); \
    } \
}

    uint8_t buffer[I2C_MAX_BUFFER_SIZE];
    size_t buffer_index = 0;
    while (command_format[0] != '\0') {
        _handle_case(int8_t, 'b', int);
        _handle_case(uint8_t, 'B', int);
        _handle_case(int16_t, 'h', int);
        _handle_case(uint16_t, 'H', int);
        _handle_case(int32_t, 'i', int32_t);
        _handle_case(uint32_t, 'I', uint32_t);
        _handle_case(float, 'f', double);
        command_format++;
    }

    i2c_master_write(i2c_handle, buffer, buffer_index, 1);
    i2c_master_stop(i2c_handle);

    // Send to I2C bus
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_cmd_begin(I2C_PORT, i2c_handle, I2C_TIMEOUT_MS / portTICK_PERIOD_MS));
    i2c_cmd_link_delete(i2c_handle);

    va_end(parameters);
}

void receive_from_i2c(uint8_t slave_addr, const char* format, ...)
{
    va_list parameters;
    va_start(parameters, format);

    // Initiate buffer
    uint8_t buffer[I2C_MAX_BUFFER_SIZE];
    size_t current_buffer_index = 0;
    memset(buffer, 0, I2C_MAX_BUFFER_SIZE);

    // Read data from slave
    i2c_cmd_handle_t i2c_handle = i2c_cmd_link_create();
    i2c_master_start(i2c_handle);
    i2c_master_write_byte(i2c_handle, (slave_addr << 1) | I2C_MASTER_READ, 0);
    i2c_master_read(i2c_handle, buffer, I2C_MAX_BUFFER_SIZE, 0);
    i2c_master_stop(i2c_handle);
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_cmd_begin(I2C_PORT, i2c_handle, I2C_TIMEOUT_MS / portTICK_PERIOD_MS));
    i2c_cmd_link_delete(i2c_handle);

    // Parse received data into varargs
    while (format[0] != '\0') {
        if (format[0] == 'h') {
            int16_t *dest = va_arg(parameters, int16_t*);
            memcpy((uint8_t*)dest, buffer + current_buffer_index, sizeof(int16_t));
            current_buffer_index += sizeof(int16_t);
        }
        format++;
    }

    va_end(parameters);
}