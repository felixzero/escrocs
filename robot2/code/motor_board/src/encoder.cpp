#include "encoder.h"
#include "encoder_isr.h"

const uint8_t pinout[3][2] = {
    {4, 7},
    {11, 12},
    {2, 13}
};

void init_encoders()
{
    for (uint8_t channel = 0; channel < 3; ++channel) {
        pinMode(pinout[channel][0], INPUT_PULLUP);
        pinMode(pinout[channel][1], INPUT_PULLUP);
    }

    noInterrupts();

    // Activate PCINT 0 & 2 on proper ports
    PCMSK0 = _BV(PCINT3) | _BV(PCINT4) | _BV(PCINT5);
    PCMSK2 = _BV(PCINT2) | _BV(PCINT4) | _BV(PCINT7);

    // Enable PCINT 0 & 2
    PCIFR = 0;
    PCICR |= _BV(PCIE0) | _BV(PCIE2);

    interrupts();
}

void read_encoders(int16_t *channel1, int16_t *channel2, int16_t *channel3)
{
   *channel1 = hall_count_1;
   *channel2 = hall_count_2;
   *channel3 = hall_count_3;
}
