#pragma once

#include <stdbool.h>

#define GPIO_CHANNEL_TRIGGER        13
#define GPIO_CHANNEL_SIDE           25
#define GPIO_CHANNEL_STATUS_LED     2

typedef enum {
    ROTARY_ENCODER_NO_EVENT,
    ROTARY_ENCODER_TURN_CW,
    ROTARY_ENCODER_TURN_CCW,
    ROTARY_ENCODER_PRESSED
} rotary_encoder_event_t;

void init_display(void);

void lcd_printf(int row, const char *format_str, ...);

rotary_encoder_event_t poll_rotary_encoder_event(void);

bool read_switch(int channel);

bool read_trigger_key_status(void);

void switch_status_led(bool state);
