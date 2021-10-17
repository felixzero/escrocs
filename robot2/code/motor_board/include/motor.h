#pragma once

#include <inttypes.h>

typedef int16_t motor_speed_t;

/// To be called at program initialization
void init_motors();

/**
 * Write the voltage value for each motor
 * @param speed: negative is CW
 */
void write_motor_speed(motor_speed_t speed1, motor_speed_t speed2, motor_speed_t speed3);
