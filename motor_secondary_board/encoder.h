#pragma once

#include <Arduino.h>

class EncoderSingleton {
public:
    void begin();
    long readChannel(char channelNumber);
};

extern EncoderSingleton Encoder;
