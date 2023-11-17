#pragma once

#include <stdint.h>
#include <stdbool.h>

extern volatile int16_t encoder_values[];

/**
 * Init the stepper control
 */
void init_steppers(void);

/**
 * Set the speed of a stepper, by setting its tick period (in units of system clock cycles)
 * I.e. 1 tick = 1/16MHz = 62.5ns
 */
void set_stepper_speed(uint8_t channel, int32_t period);

/**
 * Specify if the steppers are enabled (motors under control) or disabled (free wheeling)
 */
void set_stepper_enable(bool enabled);
bool is_stepper_enabled(void);
