#pragma once

#include <esp_types.h>
#include "driver/uart.h"

extern const uart_port_t lidar_port;

typedef enum {
    START = 0,
    LEN = 1,
    DATA = 2
} Msg_type;

void init_lidar(uint8_t rx_pin);

void scan_lidar();