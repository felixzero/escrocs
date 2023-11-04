#pragma once

#include <stdint.h>

uint8_t handle_modbus_request(
    uint8_t slave_addr,
    const uint8_t *rx_buffer,
    uint8_t *tx_buffer,
    uint8_t rx_size
);
