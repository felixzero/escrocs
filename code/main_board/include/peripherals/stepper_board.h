#pragma once

#include <stdbool.h>
#include <esp_err.h>

esp_err_t init_stepper_board(void);

void set_stepper_board_pump(unsigned int channel, bool pump_on);

/**
 * Move unipolar stepper motor to target
 * @param channel Stepper motor channel
 * @param target Position to move to
 * @param speed (in range [0, 1])
 * @param acceleration (in range [0, 1])
 */
void move_stepper_board_motor(unsigned int channel, int target, float speed, float acceleration);