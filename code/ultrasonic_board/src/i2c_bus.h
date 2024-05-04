#pragma once

#include <stdbool.h>

/**
 * Start I2C in slave mode.
 * The chip will be fully controlled by the I2C bus.
 */
void init_i2c(void);

/**
 * Check if any action from our part is required on the I2C bus.
 * Execute any relevant callback.
 */
bool poll_i2c_operations(void);
