#include "basic_io.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#define POWER_IO_PORT PORTC
#define POWER_IO_BITMASK 0b111

#define BLDC_IO_PORT PORTD
#define BLDC_PWM_PIN 3
#define BLDC_DIR_PIN 4

volatile uint16_t *servo_channel_registers[] = { &OCR1A, &OCR1B };
#define NUMBER_OF_SERVO_CHANNELS 2

void init_basic_io(void)
{
    cli();

    // Configure pins as output
    POWER_IO_PORT &= ~POWER_IO_BITMASK;
    DDRC |= POWER_IO_BITMASK;
    BLDC_IO_PORT = 0;
    DDRD |= _BV(BLDC_PWM_PIN) | _BV(BLDC_DIR_PIN);
    DDRB |= _BV(1) | _BV(2);

    // Configure timer 1 for servo control
    // Phase & freq correct PWM (non inverting) to ICR1
    // No prescaler f = 8 MHz / 65536 / 2 = 61Hz
    TCCR1A = _BV(COM1A1) | _BV(COM1B1);
    TCCR1B = _BV(WGM13) | _BV(CS10);
    ICR1 = 0xFFFF;

    // Configure timer 2 for BLDC control
    // Fast PWM (non inverting) to 255
    // No prescaler f = 8 MHz / 256 = 32kHz
    TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
    TCCR2B = _BV(CS20);

    sei();
}

uint8_t get_power_io_flags(void)
{
    return POWER_IO_PORT & POWER_IO_BITMASK;
}

void set_power_io_flags(uint8_t flag)
{
    POWER_IO_PORT = (flag & POWER_IO_BITMASK) | (POWER_IO_PORT & ~POWER_IO_BITMASK);
}

uint16_t get_servo_value(uint8_t channel)
{
    if (channel >= NUMBER_OF_SERVO_CHANNELS) {
        return 0xFF;
    }
    return *servo_channel_registers[channel];
}

void set_servo_value(uint8_t channel, uint16_t value)
{
    if (channel >= NUMBER_OF_SERVO_CHANNELS) {
        return;
    }
    *servo_channel_registers[channel] = value;
}

uint8_t get_bldc_speed(void)
{
    return OCR2B;
}

void set_bldc_speed(uint8_t speed)
{
    OCR2B = speed;
}

bool get_bldc_direction(void)
{
    return BLDC_IO_PORT & _BV(BLDC_DIR_PIN);
}

void set_bldc_direction(bool direction)
{
    if (direction) {
        BLDC_IO_PORT |= _BV(BLDC_DIR_PIN);
    } else {
        BLDC_IO_PORT &= ~_BV(BLDC_DIR_PIN);
    }
}
