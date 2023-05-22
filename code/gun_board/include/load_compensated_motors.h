#pragma once

#include <stdint.h>

void init_load_compensated_motors(void);

uint8_t get_load_compensated_motor_speed_target(uint8_t channel);
void set_load_compensated_motor_speed_target(uint8_t channel, uint8_t target);

