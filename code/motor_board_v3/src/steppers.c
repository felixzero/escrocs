#include <avr/io.h>
#include <avr/interrupt.h>

#include "steppers.h"

#define NUMBER_OF_CHANNELS 3
#define MAX_TIMER_VALUE 65536
#define PULSE_WIDTH 20
#define TCCRB_CONSTANT (_BV(WGM13) | _BV(WGM12))

static volatile uint8_t *TCCRB[] = { &TCCR1B, &TCCR3B, &TCCR4B };
static volatile uint16_t *ICR[] = { &ICR1, &ICR3, &ICR4 };

volatile int16_t encoder_values[NUMBER_OF_CHANNELS] = { 0 };

void init_steppers(void)
{
    encoder_values[0];

    // Configure control GPIO
    DDRB |= _BV(0) | _BV(1);
    DDRC |= _BV(0) | _BV(1) | _BV(2) | _BV(3);
    DDRD |= _BV(0) | _BV(1) | _BV(2);
    DDRE |= _BV(0) | _BV(1) | _BV(2);

    // Note: microstepping is disabled to avoid overheating

    // Set slow decay mode to reduce hissing noise
    DDRD |= _BV(3);

    // Activate output compare A, clear on compare, fast PWM to ICR1
    // Prescaler, variable, depending on wished period
    TCCR1A = _BV(COM1A1) | _BV(WGM11);
    TCCR1B = TCCRB_CONSTANT;
    ICR1 = 1;
    OCR1A = PULSE_WIDTH;
    TCNT1 = 0;

    TCCR3A = _BV(COM3A1) | _BV(WGM31);
    TCCR3B = TCCRB_CONSTANT;
    ICR3 = 1;
    OCR3A = PULSE_WIDTH;
    TCNT3 = 0;
    
    TCCR4A = _BV(COM4A1) | _BV(WGM41);
    TCCR4B = TCCRB_CONSTANT;
    ICR4 = 1;
    OCR4A = PULSE_WIDTH;
    TCNT4 = 0;

    // Enable overflow interrupts
    cli();
    TIMSK1 |= _BV(TOIE1);
    TIMSK3 |= _BV(TOIE3);
    TIMSK4 |= _BV(TOIE4);
    sei();
}

void set_stepper_speed(uint8_t channel, int32_t period)
{
    if (channel > 3) {
        return;
    }

    // Set CW/CCW
    if (period > 0) {
        PORTE |= _BV(channel);
    } else {
        PORTE &= ~_BV(channel);
        period = -period;
    }

    // Adjust prescaler depending on period, to increase dynamic range to 26 bits
    if (period == 0) {
        *(TCCRB[channel]) = TCCRB_CONSTANT;
        *(ICR[channel]) = 1;
    } else if (period < MAX_TIMER_VALUE) {
        *(TCCRB[channel]) = TCCRB_CONSTANT | _BV(CS10);
        *(ICR[channel]) = (uint16_t)period;
    } else if (period < 8 * MAX_TIMER_VALUE) {
        *(TCCRB[channel]) = TCCRB_CONSTANT | _BV(CS11);
        *(ICR[channel]) = (uint16_t)(period >> 3);
    } else if (period < 64 * MAX_TIMER_VALUE) {
        *(TCCRB[channel]) = TCCRB_CONSTANT | _BV(CS11) | _BV(CS10);
        *(ICR[channel]) = (uint16_t)(period >> 6);
    } else if (period < 256 * MAX_TIMER_VALUE) {
        *(TCCRB[channel]) = TCCRB_CONSTANT | _BV(CS12);
        *(ICR[channel]) = (uint16_t)(period >> 8);
    } else if (period < 1024 * MAX_TIMER_VALUE) {
        *(TCCRB[channel]) = TCCRB_CONSTANT | _BV(CS12) | _BV(CS10);
        *(ICR[channel]) = (uint16_t)(period >> 10);
    } else {
        *(TCCRB[channel]) = TCCRB_CONSTANT;
        *(ICR[channel]) = 1;
    }
}
