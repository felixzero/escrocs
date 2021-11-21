#include "wireless/httpd.h"
#include "system/task_priority.h"
#include "wireless/ota.h"
#include "wireless/httpd_handlers.h"

#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <esp_log.h>

#define TAG "httpd"

static httpd_uri_t uri_handlers[] = {
    {
        .uri = "/firmware.bin",
        .method = HTTP_PUT,
        .handler = firmware_upgrade_handler,
        .user_ctx = NULL,
    },
    {
        .uri = "/pose",
        .method = HTTP_PUT,
        .handler = set_pose_target_handler,
        .user_ctx = NULL,
    },
    {
        .uri = "/pid",
        .method = HTTP_PUT,
        .handler = set_motion_control_tuning_handler,
        .user_ctx = NULL,
    }
};

#define URI_HANDLERS_LENGTH (sizeof(uri_handlers) / sizeof(uri_handlers[0]))

void init_http_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        vTaskDelete(NULL);
    }

    for (int i = 0; i < URI_HANDLERS_LENGTH; ++i) {
        httpd_register_uri_handler(server, &uri_handlers[i]);
    }

    ESP_LOGI(TAG, "HTTP server properly initiated");

    init_ota_rebooter();
}
