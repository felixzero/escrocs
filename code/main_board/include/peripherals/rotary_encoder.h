#pragma once

typedef enum {
    ROTARY_ENCODER_NO_EVENT,
    ROTARY_ENCODER_TURN_CW,
    ROTARY_ENCODER_TURN_CCW,
    ROTARY_ENCODER_PRESSED
} rotary_encoder_event_t;

void init_rotary_encoder(void);

rotary_encoder_event_t poll_rotary_encoder_event(void);
