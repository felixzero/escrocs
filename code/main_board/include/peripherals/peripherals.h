#pragma once

#include <stdbool.h>

/**
 * Initialize the peripheral card. To be called at startup.
 */
void init_peripherals(void);

/**
 * Switch a pump on or off. Channel must be in range 0-3.
 */
void set_peripherals_pump(int channel, bool status);

/**
 * Set a servo channel.
 * @param channel (in range 0-15)
 * @param pulse_width Length of the pulse in microseconds
 */
void set_peripherals_servo_channel(int channel, int pulse_width);

/**
 * Read the end course switch inputs of one of the steppers
 * @param motor_channel Stepper motor channel (0 or 1)
 * @param pin_channel End swich channel (0 or 1)
 */
bool read_peripherals_motor_input(unsigned int motor_channel, unsigned int pin_channel);
