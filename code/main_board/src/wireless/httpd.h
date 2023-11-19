#pragma once

#include <esp_err.h>

/**
 * Initialize HTTP subsystem, including OTA
 */
esp_err_t init_http_server(void);
