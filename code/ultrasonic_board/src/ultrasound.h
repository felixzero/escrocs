#pragma once

#include <stdint.h>
#include <stdbool.h>

/**
 * Init ultrasound subsystem
 */
void init_ultrasound(void);

/**
 * Perform a ultrasound scan of a specific channel.
 * This function blocks until the echo is measured or there is timeout.
 * @param channel_number Ultrasound channel number to pulse.
 * @param value Pointer to measured time (in µs).
 * @param timeout Maximum return  waiting time (in µs).
 * @return true if a measurement is available, false if timeout.
 */
bool pulse_ultrasound(uint8_t channel_number, uint16_t *value, uint16_t timeout);
