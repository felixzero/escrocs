#include "i2c_registers.h"
#include "basic_io.h"
#include "load_compensated_motors.h"

#include <stdint.h>
#include <avr/io.h>

static uint8_t servo_1l = 0, servo_2l = 0;

uint8_t read_i2c_register(uint8_t reg)
{
    switch (reg) {
    case I2C_REG_INFO:
        return I2C_INFO_VALUE;
    case I2C_REG_POWER_IO:
        return get_power_io_flags();
    case I2C_REG_SERVO_CHANNEL_1L:
        return get_servo_value(0) & 0xFF;
    case I2C_REG_SERVO_CHANNEL_1H:
        return get_servo_value(0) >> 8;
    case I2C_REG_SERVO_CHANNEL_2L:
        return get_servo_value(1) & 0xFF;
    case I2C_REG_SERVO_CHANNEL_2H:
        return get_servo_value(1) >> 8;
    case I2C_REG_DC_MOTOR_CHANNEL_1:
        return get_load_compensated_motor_speed_target(0);
    case I2C_REG_DC_MOTOR_CHANNEL_2:
        return get_load_compensated_motor_speed_target(1);
    case I2C_REG_BLDC_SPEED:
        return get_bldc_speed();
    case I2C_REG_BLDC_DIRECTION:
        return get_bldc_direction() ? 1 : 0;
    default:
        return I2C_ERROR_VALUE;
    }
}

void write_i2c_register(uint8_t reg, uint8_t value)
{
    switch (reg)
    {
    case I2C_REG_POWER_IO:
        set_power_io_flags(value);
        return;
    case I2C_REG_SERVO_CHANNEL_1L:
        servo_1l = value;
        return;
    case I2C_REG_SERVO_CHANNEL_1H:
        set_servo_value(0, (value << 8) | servo_1l);
        return;
    case I2C_REG_SERVO_CHANNEL_2L:
        servo_2l = value;
        return;
    case I2C_REG_SERVO_CHANNEL_2H:
        set_servo_value(1, (value << 8) | servo_2l);
        return;
    case I2C_REG_DC_MOTOR_CHANNEL_1:
        set_load_compensated_motor_speed_target(0, value);
        return;
    case I2C_REG_DC_MOTOR_CHANNEL_2:
        set_load_compensated_motor_speed_target(1, value);
        return;
    case I2C_REG_BLDC_SPEED:
        set_bldc_speed(value);
        return;
    case I2C_REG_BLDC_DIRECTION:
        set_bldc_direction(value);
        return;
    case I2C_REG_INFO:
    default:
        return;
    }
}
