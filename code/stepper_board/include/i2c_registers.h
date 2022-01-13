#pragma once

#include <stdint.h>

#define I2C_REG_INFO                    0x00

#define I2C_REG_PUMP0_OUT               0x01
#define I2C_REG_PUMP1_OUT               0x02
#define I2C_REG_PUMP2_OUT               0x03

#define I2C_REG_STEPPER0_POSITION_L     0x04
#define I2C_REG_STEPPER0_POSITION_H     0x05
#define I2C_REG_STEPPER0_TARGET_L       0x06
#define I2C_REG_STEPPER0_TARGET_H       0x07
#define I2C_REG_STEPPER0_PULSE_PERIOD   0x08
#define I2C_REG_STEPPER0_ACCEL_PERIOD   0x09
#define I2C_REG_STEPPER0_MOTION_ON_OFF  0x0A

#define I2C_REG_STEPPER1_POSITION_L     0x0B
#define I2C_REG_STEPPER1_POSITION_H     0x0C
#define I2C_REG_STEPPER1_TARGET_L       0x0D
#define I2C_REG_STEPPER1_TARGET_H       0x0E
#define I2C_REG_STEPPER1_PULSE_PERIOD   0x0F
#define I2C_REG_STEPPER1_ACCEL_PERIOD   0x10
#define I2C_REG_STEPPER1_MOTION_ON_OFF  0x11

#define I2C_REG_STEPPER2_POSITION_L     0x12
#define I2C_REG_STEPPER2_POSITION_H     0x13
#define I2C_REG_STEPPER2_TARGET_L       0x14
#define I2C_REG_STEPPER2_TARGET_H       0x15
#define I2C_REG_STEPPER2_PULSE_PERIOD   0x16
#define I2C_REG_STEPPER2_ACCEL_PERIOD   0x17
#define I2C_REG_STEPPER2_MOTION_ON_OFF  0x18

/**
 * Those two functions perform the logical i2c register mapping
 */
uint8_t read_i2c_register(uint8_t reg);

void write_i2c_register(uint8_t reg, uint8_t value);
