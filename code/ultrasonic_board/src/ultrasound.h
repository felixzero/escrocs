#pragma once

#include <stdint.h>

void init_ultrasound(void);
uint8_t pulse_ultrasound(uint8_t channel_number, void (*yield)(void));
