#pragma once

#include <avr/pgmspace.h>

#define SQRT_DIFF_MAX_PRECALCULATED_VALUE 4096
#define SQRT_DIFF_DEFAULT_VALUE 256
extern const PROGMEM uint16_t sqrt_diff_precalc[];
