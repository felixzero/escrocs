#pragma once

#include <stdint.h>
#include <stddef.h>
#include <esp_err.h>

/**
 * Initialize HTTP subsystem, including OTA
 */
void init_http_server(void);

/**
 * Send a binary buffer in a WebSocket frame.
 * The buffer is sent to all connected clients.
 */
esp_err_t send_over_websocket(uint8_t *buffer, size_t length);

