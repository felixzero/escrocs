#pragma once

#include <stdbool.h>
#include <esp_err.h>

esp_err_t init_stepper_board(void);

/**
 * Open or close a pump channel
 */
void set_stepper_board_pump(unsigned int channel, bool pump_on);

/**
 * Read the pump status
 */
bool get_stepper_board_pump(unsigned int channel);

/**
 * Move unipolar stepper motor to target
 * @param channel Stepper motor channel
 * @param target Position to move to
 * @param speed (in range [0, 1])
 */
void move_stepper_board_motor(unsigned int channel, int target, float speed);

/**
 * Define the current position to be the home (0)
 * @param channel Stepper motor channel
 * @param position New value of the current position
 */
void define_stepper_board_motor_home(unsigned int channel, int position);

/**
 * Return the current stepper board position
 */
int get_stepper_board_motor_position(unsigned int channel);
