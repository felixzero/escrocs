#pragma once

#include <inttypes.h>

struct motor_pwm_data {
    uint8_t portb_values[4];
    uint8_t portd_values[4];
    uint8_t channel_values[4];
} __attribute__((packed));

volatile struct motor_pwm_data motor_pwm_buffer_1;
volatile struct motor_pwm_data motor_pwm_buffer_2;
volatile struct motor_pwm_data *current_pwm_buffer;
volatile uint8_t pwm_buffer_swap;

#define idle_pwm_buffer() (current_pwm_buffer == &motor_pwm_buffer_1 ? &motor_pwm_buffer_2 : &motor_pwm_buffer_1)
#define request_pwm_buffer_swap() (pwm_buffer_swap = 1)
