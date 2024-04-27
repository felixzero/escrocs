#include <avr/io.h>
#include <avr/interrupt.h>

#include "steppers.h"

#define NUMBER_OF_CHANNELS      3
#define MAX_TIMER_VALUE         65536
#define TCCRB_CONSTANT          (_BV(WGM13) | _BV(WGM12))

volatile int16_t encoder_values[NUMBER_OF_CHANNELS] = { 0 };

static void set_prescaler_and_period(uint8_t channel, uint8_t prescaler, uint16_t period);

void init_steppers(void)
{
    encoder_values[0];

    // Configure control GPIO
    DDRB |= _BV(0) | _BV(1);
    DDRC |= _BV(0) | _BV(1) | _BV(2) | _BV(3);
    DDRD |= _BV(0) | _BV(1) | _BV(2);
    DDRE |= _BV(0) | _BV(1) | _BV(2);
    set_stepper_enable(false);

    // Microstepping set to /32
    PORTC |= 0b110;

    // Activate output compare A, clear on compare, fast PWM to ICR1
    // Prescaler, variable, depending on wished period
    TCCR1A = _BV(COM1A0) | _BV(WGM11) | _BV(WGM10);
    TCCR1B = TCCRB_CONSTANT;
    OCR1A = 1;
    TCNT1 = 0;

    TCCR3A = _BV(COM3A0) | _BV(WGM31) | _BV(WGM30);
    TCCR3B = TCCRB_CONSTANT;
    OCR3A = 1;
    TCNT3 = 0;
    
    TCCR4A = _BV(COM4A0) | _BV(WGM41) | _BV(WGM40);
    TCCR4B = TCCRB_CONSTANT;
    OCR4A = 1;
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

    // Adjust prescaler depending on period
    if (period == 0) {
        set_prescaler_and_period(channel, TCCRB_CONSTANT, 1);
    } else if (period < 64 * MAX_TIMER_VALUE) {
        set_prescaler_and_period(channel, TCCRB_CONSTANT | _BV(CS11) | _BV(CS10), period / 64);
    }else {
        set_prescaler_and_period(channel, TCCRB_CONSTANT, 1);
    }
}

void set_stepper_enable(bool enabled)
{
    if (enabled) {
        PORTB &= ~_BV(0);
    } else {
        PORTB |= _BV(0);
    }
}

bool is_stepper_enabled(void)
{
    return PORTB & _BV(0);
}

static void set_prescaler_and_period(uint8_t channel, uint8_t prescaler, uint16_t period)
{
    switch (channel) {
    case 0:
        TCCR1B = prescaler;
        OCR1A = period;
        break;
    case 1:
        TCCR3B = prescaler;
        OCR3A = period;
        break;
    case 2:
        TCCR4B = prescaler;
        OCR4A = period;
        break;
    }
}
