#include "pumps.h"

#include <avr/io.h>

static volatile uint8_t* pump_ddr_regs[] = { &DDRB, &DDRB, &DDRC };
static volatile uint8_t* pump_ports[] = { &PORTB, &PORTB, &PORTC };
static uint8_t pump_bits[] = { _BV(0), _BV(1), _BV(3) };

#define NUMBER_OF_PORTS (sizeof(pump_bits))

void init_pumps(void)
{
    // Set all pumps as outputs, off by default
    for (uint8_t i = 0; i < NUMBER_OF_PORTS; ++i) {
        *pump_ports[i] &= ~pump_bits[i];
        *pump_ddr_regs[i] |= pump_bits[i];
    }
}

void write_pump_output(uint8_t pump_channel, bool value)
{
    if (pump_channel >= NUMBER_OF_PORTS) {
        return;
    }

    if (value) {
        *pump_ports[pump_channel] |= pump_bits[pump_channel];
    } else {
        *pump_ports[pump_channel] &= ~pump_bits[pump_channel];
    }
}

