#include "wireless/httpd_handlers.h"
#include "motion/motion_control.h"

#include <esp_log.h>
#include <cJSON.h>

#define TAG "httpd"

#define MAX_BUFFER_SIZE 256

#define LOAD_JSON_ELEMENT_TO_VAL(lvalue, key) { \
    cJSON *item = cJSON_GetObjectItem(root, key); \
    if (item == NULL || !cJSON_IsNumber(item)) { \
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Could not retrieve parameter " key "\n"); \
        return ESP_OK; \
    } \
    lvalue = item->valuedouble; \
}

esp_err_t set_position_target_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Received request to PUT /position");

    char buffer[MAX_BUFFER_SIZE];
    int length_read = httpd_req_recv(req, buffer, sizeof(buffer));
    if (length_read != req->content_len) {
        ESP_LOGI(TAG, "Error while handling PUT /position");
        return ESP_FAIL;
    }

    cJSON *root = cJSON_ParseWithLength(buffer, length_read);
    if (root == NULL) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON input\n");
        return ESP_OK;
    }

    float target_x, target_y, target_z;
    LOAD_JSON_ELEMENT_TO_VAL(target_x, "x");
    LOAD_JSON_ELEMENT_TO_VAL(target_y, "y");
    LOAD_JSON_ELEMENT_TO_VAL(target_z, "z");
    set_motion_target(target_x, target_y, target_z);

    httpd_resp_send(req, "OK\n", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t set_motion_control_tuning_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Received request to PUT /pid");

    char buffer[MAX_BUFFER_SIZE];
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
    LOAD_JSON_ELEMENT_TO_VAL(tuning.pid_p, "pid_p");
    LOAD_JSON_ELEMENT_TO_VAL(tuning.pid_i, "pid_i");
    LOAD_JSON_ELEMENT_TO_VAL(tuning.pid_d, "pid_d");
    LOAD_JSON_ELEMENT_TO_VAL(tuning.friction_threshold, "friction_threshold");
    LOAD_JSON_ELEMENT_TO_VAL(tuning.max_speed, "max_speed");
    LOAD_JSON_ELEMENT_TO_VAL(tuning.acceleration_rate, "acceleration_rate");

    set_motion_control_tuning(tuning);

    httpd_resp_send(req, "OK\n", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
