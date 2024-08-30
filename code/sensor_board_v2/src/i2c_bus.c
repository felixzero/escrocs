#include "i2c_bus.h"
#include "modbus_slave.h"

#include <stdint.h>
#include <stdbool.h>
#include <util/twi.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define I2C_ADDR 0x12
#define MULTIPLEXER_ADDR 0x70
#define I2C_CLK 100000

#define BUFFER_LENGTH 256

void init_i2c(void)
{
    // Set address
    TWAR0 = I2C_ADDR << 1;
    
    // Enable I2C
    TWCR0 =  _BV(TWEN);

    //Set clock 
    TWBR0 = ((F_CPU/I2C_CLK)-16)/2;

    // Enable 5V pullup
    PORTC |= 0b110000;
}
#define start_I2C() {TWCR0 = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); }
#define wait_I2C() { while (!(TWCR0 & _BV(TWINT))); }
#define stop_I2C() {TWCR0 = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO); }

#define resume_with_ack() { TWCR0 = _BV(TWEN) | _BV(TWINT) | _BV(TWEA); }
#define resume_without_ack() { TWCR0 = _BV(TWEN) | _BV(TWINT); }

bool start_i2c(void) {
    start_I2C();
    wait_I2C();

    if (TWSR0 & TW_STATUS_MASK != TW_START) return false;
    return true;
}

bool rep_start_i2c(void) {
    start_I2C();
    wait_I2C();
    return ( TWSR0 & TW_STATUS_MASK ) == TW_REP_START;
}

//Step ?-4 (and some of 5) of typical TWI transmission in datasheet of atmega
bool addr_i2c(uint8_t address, bool write) {
    TWDR0 = (address << 1); //Shift by 1 for Write operation
    if(!write) TWDR0 |= 1;
    resume_without_ack();

    wait_I2C();
    if(write && TWSR0 & TW_STATUS_MASK != TW_MT_SLA_ACK) return false;
    if(!write && TWSR0 & TW_STATUS_MASK != TW_MR_SLA_ACK) return false;

    return true;
}

bool write_i2c(uint8_t byte) {
    TWDR0 = byte;
    resume_without_ack();
    wait_I2C();
    return ((TWSR0 & TW_STATUS_MASK)!= TW_MT_DATA_ACK) ? false : true;
}

bool read_ack_i2c(uint8_t* byte) {
    resume_with_ack();
    wait_I2C();
    *byte = TWDR0;
    return (( TWSR0 & TW_STATUS_MASK ) != TW_MR_DATA_ACK) ? false : true;
}

bool read_nak_i2c(uint8_t* byte) {
    resume_without_ack();
    wait_I2C();
    *byte = TWDR0;
    return (( TWSR0 & TW_STATUS_MASK ) != TW_MR_DATA_NACK) ? false : true;
}


bool close_i2c(void) {
    stop_I2C();
}


