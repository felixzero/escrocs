#pragma once

#include <esp_err.h>

/**
 * Initiate I2C master bus. To be called before any I2C operation.
 */
void init_i2c_master(void);

/**
 * Send a command to a I2C slave
 * @param slave_addr I2C address of the slave
 * @param command_format Little-endian structure packing. Each character corresponds to a type.
 * b = signed byte; B = unsigned byte; h = signed 16-bits, H = unsigned 16-bits,
 * i = signed 32-bits, I = unsigned 32-bits, f = float
 * @param var_args Values of the packing
 */
void send_i2c_command(uint8_t slave_addr, const char *command_format, ...);

/**
 * Receive data from a I2C slave
 * @param slave_addr I2C address of the slave
 * @param command_format Little-endian structure packing. (See send_i2c_command for details)
 * @param var_args Pointers to values to unpack into
 */
void receive_from_i2c(uint8_t slave_addr, const char* format, ...);
