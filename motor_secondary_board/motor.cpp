#include "motor.h"

static MotorSingleton Motor;

static const char motorPWMChannels[] = { 10, 11, 3 };
static const char motorAChannels[] = { 13, 6, A2 };
static const char motorBChannels[] = { 12, 2, A3 };

void MotorSingleton::begin() {
    for (char channel : motorPWMChannels) {
        pinMode(channel, OUTPUT);
    }
    for (char channel : motorAChannels) {
        pinMode(channel, OUTPUT);
    }
    for (char channel : motorBChannels) {
        pinMode(channel, OUTPUT);
    }

    /*
     * Configure timer 1 and 2 for:
     * PWM to 255
     * Prescaler /8 (7.8 kHz PWM)
     * Output B of timer 1 (D10)
     * Output A and B of timer 2 (D11 and D3)
     * 0 duty cycle by default
     */
    TCCR1A = _BV(COM1B1) | _BV(WGM10);
    TCCR1B = _BV(CS11);
    OCR1A = 0;

    TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM20);
    TCCR2B = _BV(CS21);
    OCR2A = 0;
    OCR2B = 0;
}

void MotorSingleton::writeSpeed(char channel, int speed) {
    if (speed > 0) {
        writeChannel(channel, Mode::CounterClockWise, min(speed, 255));
    } else if (speed < 0) {
        writeChannel(channel, Mode::ClockWise, min(-speed, 255));
    } else {
        writeChannel(channel, Mode::Braking);
    }
}

void MotorSingleton::writeChannel(char channel, Mode mode, uint8_t value) {
    writeMode(channel, mode);
    writeValue(channel, mode == Mode::Braking ? 255 : value);
}

void MotorSingleton::writeMode(char channel, Mode mode) {
    switch (mode)
    {
    case Mode::ClockWise:
        digitalWrite(motorAChannels[channel], HIGH);
        digitalWrite(motorBChannels[channel], LOW);
        break;

    case Mode::CounterClockWise:
        digitalWrite(motorAChannels[channel], LOW);
        digitalWrite(motorBChannels[channel], HIGH);
        break;

    default:
        digitalWrite(motorAChannels[channel], LOW);
        digitalWrite(motorBChannels[channel], LOW);
    }
}

void MotorSingleton::writeValue(char channel, uint8_t value) {
    analogWrite(motorPWMChannels[channel], value);
}
