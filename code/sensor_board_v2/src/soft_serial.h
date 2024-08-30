#pragma once

#define UART_SOFT_DELAY_US 104 // 1/9600 * 1000000

void init_soft_serial(void);

void send_serial(const char data[]);
