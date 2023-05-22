#include "i2c.h"
#include "i2c_registers.h"

#include <stdint.h>
#include <stdbool.h>
#include <util/twi.h>
#include <avr/interrupt.h>

typedef void (*i2c_command_t)(uint8_t argc, uint8_t *argv);

static uint8_t current_reg = 0x00;
static bool is_current_reg_set = false;

void init_i2c(void)
{
    // Set address
    TWAR = I2C_ADDR << 1;
    
    // Enable I2C but not its interrupt
    TWCR = _BV(TWEA) | _BV(TWEN);

    // Enable 5V pullup
    PORTC |= 0b110000;
}

#define resume_with_ack() { TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA); }
#define resume_without_ack() { TWCR = _BV(TWEN) | _BV(TWINT); }

void poll_i2c_operations(void)
{
    switch(TW_STATUS) {
    // No actual TWI pending operation
    case TW_NO_INFO:
        return;
    // Beginning of reception - slave receiver
    case TW_SR_SLA_ACK:
        is_current_reg_set = false;
        resume_with_ack();
        break;
    // Byte received - slave receiver
    case TW_SR_DATA_ACK:
        // If it is the first byte of the transmission, assume it is register definition
        if (!is_current_reg_set) {
            current_reg = TWDR;
            is_current_reg_set = true;
        } else { // Otherwise it is a byte to write
            write_i2c_register(current_reg++, TWDR);
        }
        resume_with_ack();
        break;
    // End of reception - slave receiver
    case TW_SR_STOP:
        is_current_reg_set = false;
        resume_with_ack();
        break;
    // Beginning of transmission - slave transmitter
    case TW_ST_SLA_ACK:
    // Ack received, byte requested - slave transmitter
    case TW_ST_DATA_ACK:
        TWDR = read_i2c_register(current_reg++);
        resume_with_ack();
        break;
    // Nack received or final byte received - slave transmitter
    case TW_ST_DATA_NACK:
    case TW_ST_LAST_DATA:
        resume_with_ack();
        break;
    // Bus error - any mode
    default:
        resume_without_ack();
        break;
    }
}
