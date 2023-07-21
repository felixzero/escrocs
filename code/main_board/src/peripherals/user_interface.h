#pragma once

#include <stdbool.h>

#define GPIO_CHANNEL_TRIGGER 21
#define GPIO_CHANNEL_SIDE 22
#define GPIO_CHANNEL_BA 34
#define GPIO_CHANNEL_BB 35
#define GPIO_CHANNEL_STATUS_LED 2

typedef enum {
    ROTARY_ENCODER_NO_EVENT,
    ROTARY_ENCODER_TURN_CW,
    ROTARY_ENCODER_TURN_CCW,
    ROTARY_ENCODER_PRESSED
} rotary_encoder_event_t;

void init_user_interface(void);

void lcd_printf(int row, const char *format_str, ...);

rotary_encoder_event_t poll_rotary_encoder_event(void);

bool read_switch(int channel);

void switch_on_led(void);

