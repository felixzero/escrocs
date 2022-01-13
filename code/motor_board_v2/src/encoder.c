#include "encoder.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

volatile int16_t encoder_values[3] = {0};
static volatile uint8_t previous_ports[3] = {0};

#define PIN_ENC0A 4
#define PIN_ENC0B 5
#define PIN_ENC1A 0
#define PIN_ENC1B 1
#define PIN_ENC2A 2
#define PIN_ENC2B 4

static void update_encoder_value(volatile uint8_t port_value, uint8_t pin_enc_a, uint8_t pin_enc_b, uint8_t channel);

void init_encoder(void)
{
    // Configure all pins as input pull-up
    DDRB &= ~(_BV(PIN_ENC0A) | _BV(PIN_ENC0B));
    DDRC &= ~(_BV(PIN_ENC1A) | _BV(PIN_ENC1B));
    DDRD &= ~(_BV(PIN_ENC2A) | _BV(PIN_ENC2B));
    PORTB |= _BV(PIN_ENC0A) | _BV(PIN_ENC0B);
    PORTC |= _BV(PIN_ENC1A) | _BV(PIN_ENC1B);
    PORTD |= _BV(PIN_ENC2A) | _BV(PIN_ENC2B);

    cli();

    // Enable all three pin change interrupts
    PCIFR = 0;
    PCICR = _BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0);

    // Set pin mask for encoder inputs
    PCMSK0 = _BV(PIN_ENC0A) | _BV(PIN_ENC0B);
    PCMSK1 = _BV(PIN_ENC1A) | _BV(PIN_ENC1B);
    PCMSK2 = _BV(PIN_ENC2A) | _BV(PIN_ENC2B);

    sei();
}

ISR(PCINT0_vect) {
    update_encoder_value(PINB, PIN_ENC0A, PIN_ENC0B, 0);
}

ISR(PCINT1_vect) {
    update_encoder_value(PINC, PIN_ENC1A, PIN_ENC1B, 1);
}

ISR(PCINT2_vect) {
    update_encoder_value(PIND, PIN_ENC2A, PIN_ENC2B, 2);
}

static void update_encoder_value(uint8_t port_value, uint8_t pin_enc_a, uint8_t pin_enc_b, uint8_t channel)
{
    bool current_b = port_value & _BV(pin_enc_b);
    bool previous_a = previous_ports[channel] & _BV(pin_enc_a);

    if ((previous_a && current_b) || (!previous_a && !current_b)) {
        encoder_values[channel]--;
    } else {
        encoder_values[channel]++;
    }

    previous_ports[channel] = port_value;
}
