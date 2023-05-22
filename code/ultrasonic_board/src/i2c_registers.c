#include "i2c_registers.h"
#include "controller.h"
#include "global_definitions.h"

#include <avr/io.h>

#define INFO_VALUE 0x99
#define ERROR_VALUE 0xAA

bool can_update_led = true;
bool should_update_led = true;

uint8_t read_i2c_register(uint8_t reg)
{
    if ((reg >= I2C_REG_VALUE_CHANNEL_0) || (reg >= I2C_REG_VALUE_CHANNEL_0 + NUMBER_OF_US)) {
        return ultrasound_distances[reg - I2C_REG_VALUE_CHANNEL_0];
    }

    uint8_t channel_flags;

    switch (reg) {
    case I2C_REG_INFO:
        return INFO_VALUE;
    case I2C_REG_ENABLED_CHANNELS_1:
        channel_flags = 0;
        for (uint8_t i = 0; i < 8; ++i) {
            if (enabled_channels[i]) {
                channel_flags |= _BV(i);
            }
        }
        return channel_flags;
    case I2C_REG_ENABLED_CHANNELS_2:
        channel_flags = 0;
        for (uint8_t i = 8; i < NUMBER_OF_US; ++i) {
            if (enabled_channels[i]) {
                channel_flags |= _BV(i - 8);
            }
        }
        return channel_flags;
    case I2C_REG_CRITICAL_THRESHOLD:
        return critical_threshold_distance;
    case I2C_REG_OBSTRUCTION:
        if (can_update_led)
            should_update_led = true;
        return is_path_obstructed();
    default:
        return ERROR_VALUE;
    }
}

void write_i2c_register(uint8_t reg, uint8_t value)
{
    switch (reg) {
    case I2C_REG_ENABLED_CHANNELS_1:
        for (uint8_t i = 0; i < 8; ++i) {
            enabled_channels[i] = !!(value & _BV(i));
        }
        return;
    case I2C_REG_ENABLED_CHANNELS_2:
        for (uint8_t i = 0; i < NUMBER_OF_US; ++i) {
            enabled_channels[8 + i] = !!(value & _BV(i));
        }
        return;
    case I2C_REG_CRITICAL_THRESHOLD:
        critical_threshold_distance = value;
        return;
    }
}
