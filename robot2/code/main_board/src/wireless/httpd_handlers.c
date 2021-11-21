#include "wireless/httpd_handlers.h"
#include "motion/motion_control.h"

#include <esp_log.h>
#include <cJSON.h>

#define TAG "httpd"

#define MAX_BUFFER_SIZE 4096
static char buffer[MAX_BUFFER_SIZE];

#define LOAD_JSON_ELEMENT_TO_VAL(lvalue, key) { \
    cJSON *item = cJSON_GetObjectItem(root, key); \
    if (item == NULL || (!cJSON_IsNumber(item) && !cJSON_IsNull(item))) { \
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Could not retrieve parameter " key "\n"); \
        return ESP_OK; \
    } \
    lvalue = cJSON_GetNumberValue(item); \
}

esp_err_t set_pose_target_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Received request to PUT /pose");

    int length_read = httpd_req_recv(req, buffer, sizeof(buffer));
    if (length_read != req->content_len) {
        ESP_LOGI(TAG, "Error while handling PUT /pose");
        return ESP_FAIL;
    }

    cJSON *root = cJSON_ParseWithLength(buffer, length_read);
    if (root == NULL) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON input\n");
        return ESP_OK;
    }

    pose_t target;
    LOAD_JSON_ELEMENT_TO_VAL(target.x, "x");
    LOAD_JSON_ELEMENT_TO_VAL(target.y, "y");
    LOAD_JSON_ELEMENT_TO_VAL(target.theta, "theta");
    set_motion_target(&target);

    httpd_resp_send(req, "OK\n", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t set_motion_control_tuning_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Received request to PUT /pid");

    int length_read = httpd_req_recv(req, buffer, sizeof(buffer));
    if (length_read != req->content_len) {
        ESP_LOGI(TAG, "Error while handling PUT /pid");
        return ESP_FAIL;
    }

    cJSON *root = cJSON_ParseWithLength(buffer, length_read);
    if (root == NULL) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON input\n");
        return ESP_OK;
    }

    motion_control_tuning_t tuning;
    #define X(name, value) LOAD_JSON_ELEMENT_TO_VAL(tuning.name, #name);
    MOTION_CONTROL_TUNING_FIELDS
    #undef X
    set_motion_control_tuning(&tuning);

    httpd_resp_send(req, "OK\n", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
