#pragma once

#include <stdint.h>

/**
 * Init encoder reader subsystem
 */
void init_encoder(void);

/**
 * Current encoder values
 */
extern volatile int16_t encoder_values[];
