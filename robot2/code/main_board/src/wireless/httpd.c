#include "wireless/httpd.h"
#include "system/task_priority.h"
#include "wireless/ota.h"

#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <esp_log.h>

#define TAG "httpd"

static httpd_uri_t firmware_upgrade_put = {
    .uri = "/firmware.bin",
    .method = HTTP_PUT,
    .handler = firmware_upgrade_handler,
    .user_ctx = NULL,
};

void init_http_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        vTaskDelete(NULL);
    }

    httpd_register_uri_handler(server, &firmware_upgrade_put);
    ESP_LOGI(TAG, "HTTP server properly initiated");

    init_ota_rebooter();
}
