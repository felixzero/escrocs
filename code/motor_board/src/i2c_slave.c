#include "i2c_slave.h"

#include <inttypes.h>
#include <util/twi.h>
#include <avr/interrupt.h>

#include "motor.h"
#include "encoder.h"

#define I2C_ADDR 0x0A
#define BUFFER_LENGTH 64

typedef void (*i2c_command_t)(uint8_t argc, uint8_t *argv);

static void handle_stop(uint8_t argc, uint8_t *argv);
static void handle_set_motor_speed(uint8_t argc, uint8_t *argv);
static void handle_get_position(uint8_t argc, uint8_t *argv);

static uint8_t rx_buffer_index = 0;
static uint8_t rx_buffer[BUFFER_LENGTH];

static uint8_t tx_buffer_index = 0;
static uint8_t tx_buffer_size = 0;
static uint8_t tx_buffer[BUFFER_LENGTH];

static const i2c_command_t command_mapping[] = {
    handle_stop,                // 0x00
    handle_set_motor_speed,     // 0x01
    handle_get_position         // 0x02
};

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
#define resume_with_stop() { TWCR = _BV(TWEN) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTO); }

void poll_i2c_operations(void)
{
    switch(TW_STATUS) {
    // No actual TWI pending operation
    case TW_NO_INFO:
        return;
    // Beginning of reception - slave receiver
    case TW_SR_SLA_ACK:
        rx_buffer_index = 0;
        resume_with_ack();
        break;
    // Byte received - slave receiver
    case TW_SR_DATA_ACK:
        rx_buffer[rx_buffer_index++] = TWDR;
        resume_with_ack();
        break;
    // End of reception - slave receiver
    case TW_SR_STOP:
        if (rx_buffer[0] < sizeof(command_mapping) / sizeof(command_mapping[0])) {
            command_mapping[rx_buffer[0]](rx_buffer_index - 1, rx_buffer + 1);
        }
        rx_buffer_index = 0;
        resume_with_ack();
        break;
    // Beginning of transmission - slave transmitter
    case TW_ST_SLA_ACK:
        tx_buffer_index = 0;
    // Ack received, byte requested - slave transmitter
    case TW_ST_DATA_ACK:
        TWDR = tx_buffer[tx_buffer_index++];
        if (tx_buffer_index == tx_buffer_size) {
            resume_without_ack();
        } else {
            resume_with_ack();
        }
        break;
    // Nack received or final byte received - slave transmitter
    case TW_ST_DATA_NACK:
    case TW_ST_LAST_DATA:
        tx_buffer_index = 0;
        resume_with_ack();
        break;
    // Bus error - any mode
    default:
        resume_without_ack();
        break;
    }
}

static void handle_stop(uint8_t argc, uint8_t *argv)
{
    // Check number of arguments
    if(argc != 0) { return; }
    write_motor_speed(0, 0, 0);
}

static void handle_set_motor_speed(uint8_t argc, uint8_t *argv)
{
    // Check number of arguments
    if(argc != 3 * sizeof(motor_speed_t)) { return; }
    motor_speed_t *casted_argv = (motor_speed_t*)argv;
    write_motor_speed(casted_argv[0], casted_argv[1], casted_argv[2]);
}

static void handle_get_position(uint8_t argc, uint8_t *argv)
{
    // Check number of arguments
    if(argc != 0) { return; }
    encoder_t *casted_tx_buffer = (encoder_t*)tx_buffer;
    read_encoders(&casted_tx_buffer[0], &casted_tx_buffer[1], &casted_tx_buffer[2]);
    tx_buffer_size = 3 * sizeof(encoder_t);
    tx_buffer_index = 0;
}

