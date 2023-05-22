#include "load_compensated_motors.h"

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t *channel_registers[] = { &OCR0A, &OCR0B };
#define NUMBER_OF_CHANNELS 2

void init_load_compensated_motors(void)
{
    cli();

    // Set PD5 and PD6 as output pins
    DDRD |= _BV(5) | _BV(6);

    // Configure timer 0 for PWM output on motor pins
    // Fast PWM, non-inverting, no prescaling
    TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);
    TCCR0B = _BV(CS00);
    OCR0A = OCR0B = 0;

    sei();
}

uint8_t get_load_compensated_motor_speed_target(uint8_t channel)
{
    if (channel >= NUMBER_OF_CHANNELS) {
        return 0xFF;
    }
    return *channel_registers[channel];
}

void set_load_compensated_motor_speed_target(uint8_t channel, uint8_t target)
{
    if (channel >= NUMBER_OF_CHANNELS) {
        return;
    }
    *channel_registers[channel] = target;
}
