#pragma once

#include <stdint.h>

#define I2C_ADDR                        0x0D

#define I2C_INFO_VALUE                  0x9A
#define I2C_ERROR_VALUE                 0xAA

// Always returns I2C_INFO_VALUE
#define I2C_REG_INFO                    0x00

// Bits 0-3 control the 12V power outputs
// Bits 4-7 are unused
#define I2C_REG_POWER_IO                0x10

// Duty cycle (16 bits) for both servo/PWM channels
#define I2C_REG_SERVO_CHANNEL_1L        0x20
#define I2C_REG_SERVO_CHANNEL_1H        0x21
#define I2C_REG_SERVO_CHANNEL_2L        0x22
#define I2C_REG_SERVO_CHANNEL_2H        0x23

// Speed control (compute using formula) for the DC motor channels
#define I2C_REG_DC_MOTOR_CHANNEL_1      0x30
#define I2C_REG_DC_MOTOR_CHANNEL_2      0x31

// Speed (0-255) and direction (0 or 1) for the brushless controller
#define I2C_REG_BLDC_SPEED              0x40
#define I2C_REG_BLDC_DIRECTION          0x41

/**
 * Those two functions perform the logical i2c register mapping
 */
uint8_t read_i2c_register(uint8_t reg);

void write_i2c_register(uint8_t reg, uint8_t value);
