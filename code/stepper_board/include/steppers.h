#pragma once

#include <stdint.h>
#include <stdbool.h>

/**
 * Init stepper subsystem
 */
void init_steppers(void);

/**
 * Return the current position (compared to home) of a stepper
 * @param channel Stepper channel to query
 */
int16_t stepper_position(uint8_t channel);

/**
 * Move stepper to a given position
 * @param channel Stepper channel to move
 * @param position Target position
 * @param pulse_period Inverse of speed (1 = fastest; 255 = slowest)
 * @param acceleration_period Inverse of acceleration. Set 0 to disable ramp.
 * Otherwise (1 = fast acceleration; 255 = ultra slow acceleration)
 */
void move_stepper(uint8_t channel, int16_t position, uint8_t pulse_period, uint8_t acceleration_period);

/**
 * Abort current motion
 * @param channel Stepper channel to abort
 */
void stop_motion(uint8_t channel);

/**
 * Tell if stepper is moving
 * @param channel Stepper channel to query
 */
bool is_stepper_in_motion(uint8_t channel);