#pragma once

#include <Arduino.h>

/// To be called at program initialization
void init_motors();

/**
 * Write the voltage value for each motor
 * @param speed: in range [-1.0, 1.0]; negative is CW
 */
void write_motor_speed(float speed1, float speed2, float speed3);
