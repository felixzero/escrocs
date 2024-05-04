#include "i2c_bus.h"
#include "com_registers.h"
#include "modbus_slave.h"

#include <stdint.h>
#include <stdbool.h>
#include <util/twi.h>
#include <avr/interrupt.h>

#define I2C_ADDR 0x11
#define BUFFER_LENGTH 256

typedef void (*i2c_command_t)(uint8_t argc, uint8_t *argv);

static int rx_size = 0, tx_size = 0, tx_index = 0;
static uint8_t rx_buffer[BUFFER_LENGTH];
static uint8_t tx_buffer[BUFFER_LENGTH];

void init_i2c(void)
{
    // Set address
    TWAR0 = I2C_ADDR << 1;
    
    // Enable I2C but not its interrupt
    TWCR0 = _BV(TWEA) | _BV(TWEN);

    // Enable 5V pullup
    PORTC |= 0b110000;
}

#define resume_with_ack() { TWCR0 = _BV(TWEN) | _BV(TWINT) | _BV(TWEA); }
#define resume_without_ack() { TWCR0 = _BV(TWEN) | _BV(TWINT); }

void poll_i2c_operations(void)
{
    switch(TWSR0 & TW_STATUS_MASK) {
    // No actual TWI pending operation
    case TW_NO_INFO:
        return;
    // Beginning of reception - slave receiver
    case TW_SR_SLA_ACK:
        rx_size = 0;
        resume_with_ack();
        break;
    // Byte received - slave receiver
    case TW_SR_DATA_ACK:
        rx_buffer[rx_size++] = TWDR0;
        resume_with_ack();
        break;
    // End of reception - slave receiver
    case TW_SR_STOP:
        tx_size = handle_modbus_request(MODBUS_SLAVE_ADDR, rx_buffer, tx_buffer, rx_size);
        rx_size = 0;
        tx_index = 0;
        resume_with_ack();
        break;
    // Beginning of transmission - slave transmitter
    case TW_ST_SLA_ACK:
    // Ack received, byte requested - slave transmitter
    case TW_ST_DATA_ACK:
        if (tx_index < tx_size) {
            TWDR0 = tx_buffer[tx_index++];
        }
        resume_with_ack();
        break;
    // Nack received or final byte received - slave transmitter
    case TW_ST_DATA_NACK:
    case TW_ST_LAST_DATA:
        resume_with_ack();
        break;
    // Bus error - any mode
    default:
        resume_with_ack();
        break;
    }
}
