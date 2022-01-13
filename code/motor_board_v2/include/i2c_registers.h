#pragma once

#include <stdint.h>

#define I2C_REG_INFO            0x00
#define I2C_REG_MOTOR_PWM_0L    0x01
#define I2C_REG_MOTOR_PWM_0H    0x02
#define I2C_REG_MOTOR_PWM_1L    0x03
#define I2C_REG_MOTOR_PWM_1H    0x04
#define I2C_REG_MOTOR_PWM_2L    0x05
#define I2C_REG_MOTOR_PWM_2H    0x06
#define I2C_REG_ENCODER_0L      0x07
#define I2C_REG_ENCODER_0H      0x08
#define I2C_REG_ENCODER_1L      0x09
#define I2C_REG_ENCODER_1H      0x0A
#define I2C_REG_ENCODER_2L      0x0B
#define I2C_REG_ENCODER_2H      0x0C

/**
 * Those two functions perform the logical i2c register mapping
 */
uint8_t read_i2c_register(uint8_t reg);

void write_i2c_register(uint8_t reg, uint8_t value);
