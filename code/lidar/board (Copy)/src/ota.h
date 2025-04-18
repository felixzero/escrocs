#ifndef OTA_H
#define OTA_H

#include "esp_err.h"
#include "esp_http_server.h"

/**
 * @brief Initialize Wi-Fi in SoftAP mode.
 */
void wifi_init_softap(void);

/**
 * @brief Start the web server for OTA updates.
 */
//TODO : TEST BELOW FUNCTION : it may be not working !
void start_webserver(void);

#endif // OTA_H
