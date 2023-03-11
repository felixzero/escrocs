#pragma once

#include <stdint.h>

#define I2C_REG_INFO                    0x00
#define I2C_REG_ENABLED_CHANNELS_1      0x01
#define I2C_REG_ENABLED_CHANNELS_2      0x02
#define I2C_REG_CRITICAL_THRESHOLD      0x03
#define I2C_REG_OBSTRUCTION             0x04

#define I2C_REG_VALUE_CHANNEL_0         0x10
#define I2C_REG_VALUE_CHANNEL_1         0x11
#define I2C_REG_VALUE_CHANNEL_2         0x12
#define I2C_REG_VALUE_CHANNEL_3         0x13
#define I2C_REG_VALUE_CHANNEL_4         0x14
#define I2C_REG_VALUE_CHANNEL_5         0x15
#define I2C_REG_VALUE_CHANNEL_6         0x16
#define I2C_REG_VALUE_CHANNEL_7         0x17
#define I2C_REG_VALUE_CHANNEL_8         0x18
#define I2C_REG_VALUE_CHANNEL_9         0x19

/**
 * Those two functions perform the logical i2c register mapping
 */
uint8_t read_i2c_register(uint8_t reg);

void write_i2c_register(uint8_t reg, uint8_t value);
