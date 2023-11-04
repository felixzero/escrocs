#pragma once

#include <stdint.h>
#include <stdbool.h>

void init_led_strip(void);
void write_led_strip_values(uint16_t *values, bool *enabled, uint16_t critical_threshold, uint16_t graphical_max_value);

extern void led_strip_bit_banging();

