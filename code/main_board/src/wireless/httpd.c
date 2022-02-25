#include "wireless/httpd.h"
#include "system/task_priority.h"
#include "wireless/ota.h"
#include "actions/actions_http_handlers.h"
#include "actions/game_actions.h"
#include "actions/strategy.h"
#include "motion/motion_control.h"

#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <esp_log.h>
#include <cJSON.h>

#define TAG "httpd"

static esp_err_t set_motion_control_tuning_handler(httpd_req_t *req);

static httpd_uri_t fixed_uri_handlers[] = {
    {
        .uri = "/firmware.bin",
        .method = HTTP_PUT,
        .handler = firmware_upgrade_handler,
        .user_ctx = NULL,
    },
    {
        .uri = "/strategy",
        .method = HTTP_GET,
        .handler = get_strategy_handler,
        .user_ctx = NULL,
    },
    {
        .uri = "/strategy",
        .method = HTTP_PUT,
        .handler = put_strategy_handler,
        .user_ctx = NULL,
    },
    {
        .uri = "/strategy",
        .method = HTTP_DELETE,
        .handler = delete_strategy_handler,
        .user_ctx = NULL,
    },
    {
        .uri = "/strategy/run",
        .method = HTTP_POST,
        .handler = run_strategy_handler,
        .user_ctx = NULL,
    },
    {
        .uri = "/feedback",
        .method = HTTP_PUT,
        .handler = set_motion_control_tuning_handler,
        .user_ctx = NULL,
    }
};

#define FIXED_URI_HANDLERS_LENGTH (sizeof(fixed_uri_handlers) / sizeof(fixed_uri_handlers[0]))

void init_http_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 64;
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        vTaskDelete(NULL);
    }

    for (int i = 0; i < FIXED_URI_HANDLERS_LENGTH; ++i) {
        httpd_register_uri_handler(server, &fixed_uri_handlers[i]);
    }

    for (int i = 0; i < game_actions_uri_handlers_size(); ++i) {
        httpd_register_uri_handler(server, &get_game_actions_uri_handlers()[i]);
    }
    ESP_LOGI(TAG, "HTTP server properly initiated");

    init_ota_rebooter();
}

#define MAX_BUFFER_SIZE 4096
static char buffer[MAX_BUFFER_SIZE];

static esp_err_t set_motion_control_tuning_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Received request to PUT /feedback");

    int length_read = httpd_req_recv(req, buffer, sizeof(buffer));
    if (length_read != req->content_len) {
        ESP_LOGI(TAG, "Error while handling PUT /feedback");
        return ESP_FAIL;
    }

    cJSON *root = cJSON_ParseWithLength(buffer, length_read);
    if (root == NULL) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON input\n");
        return ESP_OK;
    }

    cJSON *item;
    (void)item;

    motion_control_tuning_t tuning;
#define X(name, value) \
    item = cJSON_GetObjectItem(root, #name); \
    if (item == NULL || (!cJSON_IsNumber(item) && !cJSON_IsNull(item))) { \
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Could not retrieve parameter " #name "\n"); \
        return ESP_OK; \
    } \
    tuning.name = cJSON_GetNumberValue(item); \
MOTION_CONTROL_TUNING_FIELDS
#undef X

    set_motion_control_tuning(&tuning);

    httpd_resp_send(req, "OK\n", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
