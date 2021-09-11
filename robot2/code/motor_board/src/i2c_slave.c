#include "i2c_slave.h"

#include <inttypes.h>
#include <util/twi.h>
#include <avr/interrupt.h>

#include "holonomic_feedback.h"

#define I2C_ADDR 0x0A
#define BUFFER_LENGTH 64

typedef void (*i2c_command_t)(uint8_t argc, float *argv);

static void handle_stop(uint8_t argc, float *argv);
static void handle_set_target(uint8_t argc, float *argv);
static void handle_set_pid_parameters(uint8_t argc, float *argv);
static void handle_get_position(uint8_t argc, float *argv);

static uint8_t rx_buffer_index = 0;
static uint8_t rx_buffer[BUFFER_LENGTH];

static uint8_t tx_buffer_index = 0;
static uint8_t tx_buffer[BUFFER_LENGTH];

static const i2c_command_t command_mapping[] = {
    handle_stop,                // 0x00
    handle_set_target,          // 0x01
    handle_set_pid_parameters,  // 0x02
    handle_get_position         // 0x03
};

void init_i2c(void)
{
    // Set address
    TWAR = I2C_ADDR << 1;
    
    // Enable I2C and its interrupt
    TWCR = _BV(TWEA) | _BV(TWEN) | _BV(TWIE);
}

ISR(TWI_vect)
{
    sei();
    switch(TW_STATUS) {
    // Beginning of reception
    case TW_SR_SLA_ACK: // 0x60
    case TW_SR_ARB_LOST_SLA_ACK: // 0x68
        rx_buffer_index = 0;
        TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
        break;
    // Data received
    case TW_SR_DATA_ACK: // 0x80
        rx_buffer[rx_buffer_index++] = TWDR;
        TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
        break;
    // End of transmission
    case TW_SR_STOP: // 0xA0
        if (rx_buffer[0] < sizeof(command_mapping) / sizeof(command_mapping[0])) {
            command_mapping[rx_buffer[0]](rx_buffer_index - 1, (float*)(rx_buffer + 1));
        }
        // callback
        rx_buffer_index = 0;
        TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT);
      break;
    // Received nack from master
    case TW_SR_DATA_NACK: // 0x88
        TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
        break;
    }
}

static void handle_stop(uint8_t argc, float *argv)
{
    // Check number of arguments
    if(argc != 0) { return; }
    holonomic_stop();
}

static void handle_set_target(uint8_t argc, float *argv)
{
    // Check number of arguments
    if(argc != 3) { return; }
    set_holonomic_feedback_target(argv[0], argv[1], argv[2]);
}

static void handle_set_pid_parameters(uint8_t argc, float *argv)
{
    // Check number of arguments
    if(argc != 3) { return; }
    set_holonomic_pid_parameters(argv[0], argv[1], argv[2]);
}

static void handle_get_position(uint8_t argc, float *argv)
{
    // Check number of arguments
    if(argc != 0) { return; }
    float *casted_tx_buffer = (float*)tx_buffer;
    get_holonomic_position(&casted_tx_buffer[0], &casted_tx_buffer[1], &casted_tx_buffer[2]);
    tx_buffer_index = 3 * sizeof(float);
}

