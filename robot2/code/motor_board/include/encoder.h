#pragma once

#include <inttypes.h>

typedef int16_t encoder_t;

/// To be called at program initialization
void init_encoders();


/**
 * Read the encoder value for each channel
 * @param channel: output of each channel
 */
void read_encoders(encoder_t *channel1, encoder_t *channel2, encoder_t *channel3);
