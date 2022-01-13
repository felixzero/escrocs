#pragma once

#include <stdint.h>
#include <stdbool.h>

/**
 * Init pump subsystem
 */
void init_pumps(void);

/**
 * Switch on or off a pump
 */
void write_pump_output(uint8_t pump_channel, bool value);
