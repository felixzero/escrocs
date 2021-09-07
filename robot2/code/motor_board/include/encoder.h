#pragma once

#include <Arduino.h>

/// To be called at program initialization
void init_encoders();


/**
 * Read the encoder value for each channel
 * @param channel: output of each channel
 */
void read_encoders(int16_t *channel1, int16_t *channel2, int16_t *channel3);
