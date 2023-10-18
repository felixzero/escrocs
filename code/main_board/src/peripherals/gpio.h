#pragma once

#include <stdbool.h>

#define GPIO_CHANNEL_TRIGGER 21
#define GPIO_CHANNEL_SIDE 22
#define GPIO_CHANNEL_BA 34
#define GPIO_CHANNEL_BB 35
#define GPIO_CHANNEL_STATUS_LED 2

void init_gpio(void);

bool read_switch(int channel);

void switch_on_led(void);
