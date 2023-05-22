#pragma once

#include <stdint.h>
#include <stdbool.h>

void init_basic_io(void);

uint8_t get_power_io_flags(void);
void set_power_io_flags(uint8_t flag);

uint16_t get_servo_value(uint8_t channel);
void set_servo_value(uint8_t channel, uint16_t value);

uint8_t get_bldc_speed(void);
void set_bldc_speed(uint8_t speed);
bool get_bldc_direction(void);
void set_bldc_direction(bool direction);
