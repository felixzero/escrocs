#pragma once

#include <stdint.h>

#include "../common/radio_common.h"

/* To be called first at program startup */
int radio_emitter_initialize(void);

/* To be called after all transmissions */
void radio_emitter_terminate(void);

/* Check if radio had finished transmission */
int radio_is_packet_sent(void);

/* Start transmission of one frame.
 * The frame must be PAYLOAD_LENGTH long */
void radio_emitter_send_packet(radio_payload_t *packet);

/* Send reset pulse to the radio */
void send_reset_gpio(void);

