#include "global_definitions.h"
#include "ultrasound.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define PULSE_TIME_US 10
#define TICKS_RETURN_TIMEOUT 256
#define TICKS_PULSE_TIMEOUT 256

static volatile uint8_t* DDR_LOOKUP[] = { &DDRC, &DDRC, &DDRC, &DDRC, &DDRD, &DDRD, &DDRD, &DDRD, &DDRD, &DDRD };
static volatile uint8_t* PORT_LOOKUP[] = { &PORTC, &PORTC, &PORTC, &PORTC, &PORTD, &PORTD, &PORTD, &PORTD, &PORTD, &PORTD };
static volatile uint8_t* PIN_LOOKUP[] = { &PINC, &PINC, &PINC, &PINC, &PIND, &PIND, &PIND, &PIND, &PIND, &PIND };
static const uint8_t BV_LOOKUP[] = { 0, 1, 2, 3, 2, 3, 4, 5, 6, 7 };

void init_ultrasound(void)
{
    // Configure 8-bits timer counter 1 with 1/256 prescaler (31.25 kHz) and no output
    TCCR1A = 0;
    TCCR1B = _BV(CS12);
}

uint8_t pulse_ultrasound(uint8_t channel_number, void (*yield)(void))
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
    while (!(*PIN_LOOKUP[channel_number] & _BV(BV_LOOKUP[channel_number])) && (TCNT1 < TICKS_RETURN_TIMEOUT))
        yield();
    if (TCNT1 >= TICKS_RETURN_TIMEOUT) {
        return 255;
    }
    TCNT1 = 0;

    // Wait until echo pulse stops
    while ((*PIN_LOOKUP[channel_number] & _BV(BV_LOOKUP[channel_number])) && (TCNT1 < TICKS_PULSE_TIMEOUT))
        yield();
    return (TCNT1 >= TICKS_PULSE_TIMEOUT) ? 255 : TCNT1L;
} 
