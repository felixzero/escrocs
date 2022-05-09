#include "i2c_registers.h"
#include "pumps.h"
#include "steppers.h"

#include <avr/io.h>
#include <stdbool.h>

#define INFO_VALUE 0x99
#define BAD_REG_VALUE 0xFF

struct stepper_config {
    int16_t target;
    uint8_t pulse_period;
    uint8_t acceleration_period;
};

static struct stepper_config stepper_configs[3];
static bool pump_values[3];

uint8_t read_i2c_register(uint8_t reg)
{
    switch (reg) {
    case I2C_REG_INFO:
        return INFO_VALUE;

#define CHANNEL_READ_DEFINITION(CHANNEL)                          \
    case I2C_REG_PUMP##CHANNEL##_OUT:                             \
        return pump_values[CHANNEL];                              \
    case I2C_REG_STEPPER##CHANNEL##_POSITION_L:                   \
        return (uint8_t)stepper_position(CHANNEL);                \
    case I2C_REG_STEPPER##CHANNEL##_POSITION_H:                   \
        return (uint8_t)(stepper_position(CHANNEL) >> 8);         \
    case I2C_REG_STEPPER##CHANNEL##_TARGET_L:                     \
        return (uint8_t)stepper_configs[CHANNEL].target;          \
    case I2C_REG_STEPPER##CHANNEL##_TARGET_H:                     \
        return (uint8_t)(stepper_configs[CHANNEL].target >> 8);   \
    case I2C_REG_STEPPER##CHANNEL##_PULSE_PERIOD:                 \
        return stepper_configs[CHANNEL].pulse_period;             \
    case I2C_REG_STEPPER##CHANNEL##_MOTION_ON_OFF:                \
        return is_stepper_in_motion(CHANNEL);

    CHANNEL_READ_DEFINITION(0)
    CHANNEL_READ_DEFINITION(1)
    CHANNEL_READ_DEFINITION(2)

    default:
        return BAD_REG_VALUE;
    }
}

void write_i2c_register(uint8_t reg, uint8_t value)
{
    switch (reg) {
#define CHANNEL_WRITE_DEFINITION(CHANNEL)                                           \
    case I2C_REG_PUMP##CHANNEL##_OUT:                                               \
        write_pump_output(CHANNEL, value);                                          \
        return;                                                                     \
    case I2C_REG_STEPPER##CHANNEL##_POSITION_L:                                     \
        overwrite_stepper_position(                                                 \
            CHANNEL,                                                                \
            (read_i2c_register(I2C_REG_STEPPER##CHANNEL##_POSITION_H) << 8) | value \
        );                                                                          \
        return;                                                                     \
    case I2C_REG_STEPPER##CHANNEL##_POSITION_H:                                     \
        overwrite_stepper_position(                                                 \
            CHANNEL,                                                                \
            (value << 8) | read_i2c_register(I2C_REG_STEPPER##CHANNEL##_POSITION_L) \
        );                                                                          \
        return;                                                                     \
    case I2C_REG_STEPPER##CHANNEL##_TARGET_L:                                       \
        *((uint8_t*)&stepper_configs[CHANNEL].target) = value;                      \
        return;                                                                     \
    case I2C_REG_STEPPER##CHANNEL##_TARGET_H:                                       \
        *((uint8_t*)&stepper_configs[CHANNEL].target + 1) = value;                  \
        return;                                                                     \
    case I2C_REG_STEPPER##CHANNEL##_PULSE_PERIOD:                                   \
        stepper_configs[CHANNEL].pulse_period = value;                              \
        return;                                                                     \
    case I2C_REG_STEPPER##CHANNEL##_MOTION_ON_OFF:                                  \
        if (value) {                                                                \
            move_stepper(                                                           \
                CHANNEL,                                                            \
                stepper_configs[CHANNEL].target,                                    \
                stepper_configs[CHANNEL].pulse_period                               \
            );                                                                      \
        } else {                                                                    \
            stop_motion(CHANNEL);                                                   \
        }                                                                           \
        return;

    CHANNEL_WRITE_DEFINITION(0)
    CHANNEL_WRITE_DEFINITION(1)
    CHANNEL_WRITE_DEFINITION(2)

    default:
        return;
    }
}
