#include "stepper_board.h"
#include "system/i2c_master.h"

#include <esp_log.h>
#include <math.h>

#define TAG "Stepper board"
#define I2C_ADDR 0x0B
#define NUMBER_OF_PUMPS 3
#define NUMBER_OF_MOTORS 3

static float fclampf(float x, float a, float b);

esp_err_t init_stepper_board(void)
{
    ESP_LOGI(TAG, "Initializing servo board subsystem.");

    uint8_t info_register_value = read_i2c_register(I2C_PORT_PERIPH, I2C_ADDR, 0x00);
    if (info_register_value != 0x99) {
        ESP_LOGE(TAG, "Failing to reach servo board");
        return ESP_FAIL;
    }

    for (int i = 0; i < NUMBER_OF_PUMPS; ++i) {
        set_stepper_board_pump(i, false);
    }

    return ESP_OK;
}

void set_stepper_board_pump(unsigned int channel, bool pump_on)
{
    assert(channel < NUMBER_OF_PUMPS);
    if (pump_on) {
        ESP_LOGI(TAG, "Switching on pump %d", channel);
    } else {
        ESP_LOGI(TAG, "Switching off pump %d", channel);
    }

    write_i2c_register(I2C_PORT_PERIPH, I2C_ADDR, 0x01 + channel, pump_on);
}

bool get_stepper_board_pump(unsigned int channel)
{
    assert(channel < NUMBER_OF_PUMPS);
    return read_i2c_register(I2C_PORT_PERIPH, I2C_ADDR, 0x01 + channel);
}

void move_stepper_board_motor(unsigned int channel, int target, float speed)
{
    assert(channel < NUMBER_OF_MOTORS);
    uint8_t buffer[5];
    buffer[0] = 0x06 + channel * 6; // I2C address
    *((int16_t*)&buffer[1]) = (int16_t)target; // Position target
    buffer[3] = (uint8_t)floorf(fclampf(1.0 / speed, 1.0, 255.0)); // Speed
    buffer[4] = 1;
    ESP_LOGI(TAG, "Moving stepper %d to target %d (speed=%f)", channel, target, speed);
    send_to_i2c(I2C_PORT_PERIPH, I2C_ADDR, buffer, sizeof(buffer));
}

void define_stepper_board_motor_home(unsigned int channel, int position)
{
    write_i2c_register(I2C_PORT_PERIPH, I2C_ADDR, 0x0A + channel * 5, 0);
    write_i2c_register(I2C_PORT_PERIPH, I2C_ADDR, 0x04 + channel * 5, position & 0xFF);
    write_i2c_register(I2C_PORT_PERIPH, I2C_ADDR, 0x05 + channel * 5, position >> 8);
}

int get_stepper_board_motor_position(unsigned int channel)
{
    return (
        read_i2c_register(I2C_PORT_PERIPH, I2C_ADDR, 0x04 + channel * 5)
        | (read_i2c_register(I2C_PORT_PERIPH, I2C_ADDR, 0x05 + channel * 5) << 8)
    );
}

static float fclampf(float x, float a, float b)
{
    if (x < a) {
        return a;
    } else if (x > b) {
        return b;
    }
    return x;
}
