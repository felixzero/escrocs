#include "encoder.h"

static EncoderSingleton Encoder;
static volatile struct {
    bool hallSensorAPreviousValue;
    bool hallSensorBPreviousValue;
    long count;
} channelTicks[3];

#define ENCODER_ISR(channel, vect, portreg, pin1, pin2) \
    ISR (vect) { \
        bool hallSensorA = portreg & _BV(pin1); \
        bool hallSensorB = portreg & _BV(pin2); \
        bool increment = (hallSensorA && channelTicks[channel].hallSensorBPreviousValue) \
        || (!hallSensorA && !channelTicks[channel].hallSensorBPreviousValue); \
        channelTicks[channel].count += increment ? 1 : -1; \
        channelTicks[channel].hallSensorAPreviousValue = hallSensorA; \
        channelTicks[channel].hallSensorBPreviousValue = hallSensorB; \
    }

ENCODER_ISR(0, PCINT0_vect, PINB, PINB0, PINB1)
ENCODER_ISR(1, PCINT2_vect, PIND, PIND5, PIND4)
ENCODER_ISR(2, PCINT1_vect, PINC, PINC1, PINC0)

void EncoderSingleton::begin() {
    noInterrupts();

    // Channel 0 configuration
    PCMSK0 |= _BV(PCINT0) | _BV(PCINT1);
    PORTB |= _BV(PORTB0) | _BV(PORTB1);
    DDRB &= ~_BV(DDB0) & ~_BV(DDB1);
    // Channel 1 configuration
    PCMSK2 |= _BV(PCINT5) | _BV(PCINT4);
    PORTD |= _BV(PORTD5) | _BV(PORTD4);
    DDRD &= ~_BV(DDD5) & ~_BV(DDD4);
    // Channel 2 configuration
    PCMSK1 |= _BV(PCINT1) | _BV(PCINT0);
    PORTC |= _BV(PORTC1) | _BV(PORTC0);
    DDRC &= ~_BV(DDC1) & ~_BV(DDC0);

    // Enable all PCINT
    PCIFR = 0;
    PCICR |= _BV(PCIE0) | _BV(PCIE1) | _BV(PCIE2);

    interrupts();
}

long EncoderSingleton::readChannel(char channelNumber) {
    return channelTicks[channelNumber].count;
}
