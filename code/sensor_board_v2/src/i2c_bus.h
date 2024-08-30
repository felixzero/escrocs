#pragma once

#include <stdbool.h>
#include <stdint.h>


/**
 * Start I2C in master mode.
 * The chip will control via I2C bus.
 */
void init_i2c(void);

bool start_i2c(void);
/* to use when switch write/read and/or i2c address  */
bool rep_start_i2c(void);

/*
 * Communicate with I2C address, either write or read mode
*/
bool addr_i2c(uint8_t address, bool write);

bool write_i2c(uint8_t byte);

bool read_ack_i2c(uint8_t* byte);

bool read_nak_i2c(uint8_t* byte);

bool close_i2c(void);


/**
 * Check if any action from our part is required on the I2C bus.
 * Execute any relevant callback.
 */
void poll_i2c_operations(void);

/* Workflow  for write & read :
    start_i2c
    addr_i2c(address, 1)
    write_i2c(reg)
    rep_start_i2c
    addr_i2c(address, 0)
    read_ack_i2c
    close_i2c
*/
