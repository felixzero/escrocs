#pragma once

#include <esp_http_server.h>

/**
 * HTTP handler for PUT /firmware.bin
 */
esp_err_t firmware_upgrade_handler(httpd_req_t *req);

/**
 * Initialize automatic reboot task
 */
void init_ota_rebooter(void);
