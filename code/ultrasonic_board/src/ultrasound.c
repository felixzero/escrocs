#include "global_definitions.h"
#include "ultrasound.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define PULSE_TIME_US 10

static volatile uint8_t* DDR_LOOKUP[] = { &DDRC, &DDRC, &DDRC, &DDRC, &DDRD, &DDRD, &DDRD, &DDRD, &DDRD, &DDRD };
static volatile uint8_t* PORT_LOOKUP[] = { &PORTC, &PORTC, &PORTC, &PORTC, &PORTD, &PORTD, &PORTD, &PORTD, &PORTD, &PORTD };
static volatile uint8_t* PIN_LOOKUP[] = { &PINC, &PINC, &PINC, &PINC, &PIND, &PIND, &PIND, &PIND, &PIND, &PIND };
static const uint8_t BV_LOOKUP[] = { 0, 1, 2, 3, 2, 3, 4, 5, 6, 7 };

void init_ultrasound(void)
{
    // Configure timer 1 in normal mode (to 0xFFFF)
    // 1/8 prescaler (1 MHz), no PWM output
    TCCR1A = 0;
    TCCR1B = _BV(CS11);
}

bool pulse_ultrasound(uint8_t channel_number, uint16_t *value, uint16_t timeout)
{
    // Set pin as output
    *DDR_LOOKUP[channel_number] |= _BV(BV_LOOKUP[channel_number]);

    // Pulse pin
    *PORT_LOOKUP[channel_number] |= _BV(BV_LOOKUP[channel_number]);
    _delay_us(PULSE_TIME_US);
    *PORT_LOOKUP[channel_number] &= ~_BV(BV_LOOKUP[channel_number]);

    // Set pin as input
    *DDR_LOOKUP[channel_number] &= ~_BV(BV_LOOKUP[channel_number]);

    // Wait until echo pulse starts
    TCNT1 = 0;
    while (!(*PIN_LOOKUP[channel_number] & _BV(BV_LOOKUP[channel_number])) && (TCNT1 < timeout));
    if (TCNT1 >= timeout) {
        return false;
    }
    TCNT1 = 0;

    // Wait until echo pulse stops
    while ((*PIN_LOOKUP[channel_number] & _BV(BV_LOOKUP[channel_number])) && (TCNT1 < timeout));
    uint16_t timer_value = TCNT1;
    if (timer_value < timeout) {
        *value = timer_value;
        return true;
    }

    return false;
}
