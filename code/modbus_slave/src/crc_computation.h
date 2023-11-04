#pragma once

#include <stddef.h>
#include <stdint.h>

uint16_t compute_crc(const uint8_t *message, uint8_t len);
