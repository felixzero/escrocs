#include "ultrasonic_board.h"
#include "system/i2c_master.h"

#include <esp_log.h>
#include <math.h>

#define TAG "Ultrasonic board"
#define I2C_ADDR 0x0C

#define I2C_REG_INFO                    0x00
#define I2C_REG_ENABLED_CHANNELS_1      0x01
#define I2C_REG_ENABLED_CHANNELS_2      0x02
#define I2C_REG_CRITICAL_THRESHOLD      0x03
#define I2C_REG_OBSTRUCTION             0x04
#define I2C_REG_CHANNELS                0x10

#define OBSTRUCTION_DISTANCE            40
#define ARBITRARY_UNIT_TO_M             0.001

esp_err_t init_ultrasonic_board(void)
{
    ESP_LOGI(TAG, "Initializing ultrasonic detection subsystem.");

    uint8_t info_register_value = read_i2c_register(I2C_PORT_MOTOR, I2C_ADDR, I2C_REG_INFO);
    if (info_register_value != 0x99) {
        ESP_LOGE(TAG, "Failing to reach ultrasonic board");
        return ESP_FAIL;
    }

    disable_ultrasonic_detection();
    write_i2c_register(I2C_PORT_MOTOR, I2C_ADDR, I2C_REG_OBSTRUCTION, OBSTRUCTION_DISTANCE);
    return ESP_OK;
}

void set_ultrasonic_scan_angle(float min_angle, float max_angle)
{
    uint16_t bit_field = 0;

    for (
        int channel = floorf(min_angle * NUMBER_OF_SECTORS / 2 / M_PI);
        channel <= ceilf(max_angle * NUMBER_OF_SECTORS / 2 / M_PI);
        channel++
    ) {
        bit_field |= (1 << ((2 * NUMBER_OF_SECTORS - 1 - channel) % NUMBER_OF_SECTORS));
    }

    write_i2c_register(I2C_PORT_MOTOR, I2C_ADDR, I2C_REG_ENABLED_CHANNELS_1, bit_field & 0xFF);
    write_i2c_register(I2C_PORT_MOTOR, I2C_ADDR, I2C_REG_ENABLED_CHANNELS_2, bit_field >> 8);
}

void request_full_ultrasonic_scan(void)
{
    write_i2c_register(I2C_PORT_MOTOR, I2C_ADDR, I2C_REG_ENABLED_CHANNELS_1, 0xFF);
    write_i2c_register(I2C_PORT_MOTOR, I2C_ADDR, I2C_REG_ENABLED_CHANNELS_2, 0xFF);
}

void wait_for_next_full_scan(void)
{
    // FIXME: Actually synchronize
    vTaskDelay(10);
}

bool ultrasonic_has_obstacle(void)
{
    return read_i2c_register(I2C_PORT_MOTOR, I2C_ADDR, I2C_REG_OBSTRUCTION);
}

void disable_ultrasonic_detection(void)
{
    write_i2c_register(I2C_PORT_MOTOR, I2C_ADDR, I2C_REG_ENABLED_CHANNELS_1, 0x00);
    write_i2c_register(I2C_PORT_MOTOR, I2C_ADDR, I2C_REG_ENABLED_CHANNELS_2, 0x00);
}


float ultrasonic_get_distance_by_sector(int channel)
{
    return ARBITRARY_UNIT_TO_M * read_i2c_register(I2C_PORT_MOTOR, I2C_ADDR, I2C_REG_CHANNELS + channel);
}

float ultrasonic_get_angle_by_sector(int channel)
{
    return (channel + 0.5) * 2* M_PI / NUMBER_OF_SECTORS;
}
