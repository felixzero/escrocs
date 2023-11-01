#pragma once

#include <stdint.h>

extern volatile int16_t encoder_values[];

void init_steppers(void);

void set_stepper_speed(uint8_t channel, int32_t period);
