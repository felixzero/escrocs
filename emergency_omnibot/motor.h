#pragma once

#include <Arduino.h>

class MotorSingleton {
public:
    enum class Mode { Idle, ClockWise, CounterClockWise, Braking };

    void begin();
    void writeSpeed(char channel, int speed);
    void writeChannel(char channel, Mode mode, uint8_t value = 0);

private:
    void writeMode(char channel, Mode mode);
    void writeValue(char channel, uint8_t value);
};

extern MotorSingleton Motor;
