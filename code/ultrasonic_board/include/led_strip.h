#pragma once

#include <stdint.h>
#include <stdbool.h>

void init_led_strip(void);
void write_led_strip_values(uint8_t *values, bool *enabled, uint8_t critical_threshold);

extern void led_strip_bit_banging();
extern volatile uint8_t led_strip_payload[];
