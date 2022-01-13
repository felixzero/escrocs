#include "motor_pwm.h"

#include <avr/io.h>
#include <stdlib.h>

static void write_motor_register(volatile uint8_t *ocra, volatile uint8_t *ocrb, motor_pwm_t value);

void init_motor_pwm(void)
{
    // Configure PWM pins as outputs
    DDRB |= _BV(1) | _BV(2) | _BV(3);
    DDRD |= _BV(3) | _BV(5) | _BV(6);

    // Configure all prescalers to /8
    // Yielding 7.8 kHz PWM output
    TCCR0B = _BV(CS01);
    TCCR1B = _BV(CS11);
    TCCR2B = _BV(CS21);

    // Set high output by default
    OCR0A = OCR0B = OCR1A = OCR1B = OCR2A = OCR2B = 255;

    // Set all channels in phase-correct PWM mode and activate PWM on both output
    TCCR0A = _BV(WGM00) | _BV(COM0A1) | _BV(COM0B1);
    TCCR1A = _BV(WGM10) | _BV(COM1A1) | _BV(COM1B1);
    TCCR2A = _BV(WGM20) | _BV(COM2A1) | _BV(COM2B1);
}

void write_motor_channel(motor_channel_t channel, motor_pwm_t value)
{
    switch (channel) {
    case MOTOR_CHANNEL_0:
        write_motor_register(&OCR0A, &OCR0B, value);
        break;
    case MOTOR_CHANNEL_1:
        write_motor_register(&OCR1AL, &OCR1BL, value);
        break;
    case MOTOR_CHANNEL_2:
        write_motor_register(&OCR2A, &OCR2B, value);
        break;
    }
}

static void write_motor_register(volatile uint8_t *ocra, volatile uint8_t *ocrb, motor_pwm_t value)
{
    if (value >= 0) {
        *ocra = 255 - (uint8_t)abs(value);
        *ocrb = 255;
    } else {
        *ocrb = 255 - (uint8_t)abs(value);
        *ocra = 255;
    }
}
