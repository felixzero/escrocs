#pragma once

#include <driver/i2c.h>
#include <esp_err.h>

#define I2C_PORT_MOTOR I2C_NUM_0
#define I2C_PORT_PERIPH I2C_NUM_1

/**
 * Initiate I2C master bus. To be called before any I2C operation.
 */
esp_err_t init_i2c_master(void);

/**
 * Send a command to a I2C slave
 * @param port (I2C_PORT_MOTOR or I2C_PORT_PERIPH)
 * @param slave_addr I2C address of the slave
 * @param buffer Data to send
 * @param length Packet size
 */
void send_to_i2c(int port, uint8_t slave_addr, void *buffer, size_t length);

/**
 * Receive data from a I2C slave
 * @param port (I2C_PORT_MOTOR or I2C_PORT_PERIPH)
 * @param slave_addr I2C address of the slave
 * @param buffer Destination buffer to write into. Must be at least "length" long
 * @param length Packet size
 */
void request_from_i2c(int port, uint8_t slave_addr, void *buffer, size_t length);

/**
 * Convenience functions for device that works with 8-bit I2C registers
 * @param port (I2C_PORT_MOTOR or I2C_PORT_PERIPH)
 * @param slave_addr I2C address of the slave
 * @param register_addr Address of the register (1st byte of the communication)
 * @param value Value to write/read (2nd byte of the communication)
 */
void write_i2c_register(int port, uint8_t slave_addr, uint8_t register_addr, uint8_t value);
uint8_t read_i2c_register(int port, uint8_t slave_addr, uint8_t register_addr);

