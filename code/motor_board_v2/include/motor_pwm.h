#pragma once

#include <stdint.h>

typedef uint8_t motor_channel_t;
typedef int16_t motor_pwm_t;

#define MOTOR_CHANNEL_0     0
#define MOTOR_CHANNEL_1     1
#define MOTOR_CHANNEL_2     2

/**
 * Initialize the motor PWM subsystem
 */
void init_motor_pwm(void);

/**
 * Set PWM on one of the channels
 * @param channel motor channel, in range [0, 3[
 * @param value 8-bit PWM value with sign (in range [-255, 255])
 */
void write_motor_channel(motor_channel_t channel, motor_pwm_t value);
