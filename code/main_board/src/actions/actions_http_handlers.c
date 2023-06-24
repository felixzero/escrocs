#include "game_actions.h"

#include <esp_err.h>
#include <esp_log.h>
#include <esp_http_server.h>
#include <cJSON.h>
#include <math.h>

#define TAG "httpd"

#define MAX_BUFFER_SIZE 4096
static char buffer[MAX_BUFFER_SIZE];

#define X(action_name, function, ARGUMENTS, OUTPUT) \
static esp_err_t game_action_ ## action_name ## _http_handler(httpd_req_t *req) \
{   \
    ESP_LOGI(TAG, "Received request to POST /action/" # action_name); \
    int length_read = httpd_req_recv(req, buffer, sizeof(buffer)); \
    if (length_read != req->content_len) { \
        ESP_LOGI(TAG, "Error while handling POST /action/" # action_name); \
        return ESP_FAIL; \
    } \
    cJSON *root = cJSON_ParseWithLength(buffer, length_read); \
    if (root == NULL) { \
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON input\n"); \
        return ESP_OK; \
    } \
    cJSON *item; \
    (void)item; \
    struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(action_name) args; \
ARGUMENTS \
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(action_name) output = game_action_ ## action_name(args); \
    root = cJSON_CreateObject(); \
    cJSON *entry; \
    (void)entry; \
    (void)output; \
OUTPUT \
    char *json_string = cJSON_Print(root); \
    \
    httpd_resp_send(req, json_string, HTTPD_RESP_USE_STRLEN); \
    return ESP_OK; \
}

#define X_FLOAT_ARGS(parameter_name) \
item = cJSON_GetObjectItem(root, #parameter_name); \
if (item == NULL || (!cJSON_IsNumber(item) && !cJSON_IsNull(item))) { \
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Could not retrieve parameter " #parameter_name "\n"); \
    return ESP_OK; \
} \
args.parameter_name = cJSON_IsNull(item) ? NAN : cJSON_GetNumberValue(item);

#define X_INT_ARGS(parameter_name) \
item = cJSON_GetObjectItem(root, #parameter_name); \
if (item == NULL || !cJSON_IsNumber(item)) { \
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Could not retrieve parameter " #parameter_name "\n"); \
    return ESP_OK; \
} \
args.parameter_name = cJSON_GetNumberValue(item);

#define X_BOOL_ARGS(parameter_name) \
item = cJSON_GetObjectItem(root, #parameter_name); \
if (item == NULL || !cJSON_IsBool(item)) { \
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Could not retrieve parameter " #parameter_name "\n"); \
    return ESP_OK; \
} \
args.parameter_name = cJSON_IsTrue(item);

#define X_STR_ARGS(parameter_name) \
item = cJSON_GetObjectItem(root, #parameter_name); \
if (item == NULL || !cJSON_IsString(item)) { \
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Could not retrieve parameter " #parameter_name "\n"); \
    return ESP_OK; \
} \
args.parameter_name = cJSON_GetStringValue(item);

#define X_FLOAT_OUTPUT(parameter_name) \
entry = cJSON_CreateNumber(output.parameter_name); \
cJSON_AddItemToObject(root, #parameter_name, entry);

#define X_INT_OUTPUT(parameter_name) \
entry = cJSON_CreateNumber(output.parameter_name); \
cJSON_AddItemToObject(root, #parameter_name, entry);

#define X_BOOL_OUTPUT(parameter_name) \
entry = cJSON_CreateBool(output.parameter_name); \
cJSON_AddItemToObject(root, #parameter_name, entry);

DEFINE_GAME_ACTION_FUNCTIONS
#undef X

// Define URI handler list

#define X(action_name, function, ARGUMENTS, OUTPUT) \
    { \
        .uri = "/action/" # action_name, \
        .method = HTTP_POST, \
        .handler = game_action_ ## action_name ## _http_handler, \
        .user_ctx = NULL, \
    },

httpd_uri_t game_actions_uri_handlers_array[] = {
DEFINE_GAME_ACTION_FUNCTIONS
};
#undef X

httpd_uri_t *get_game_actions_uri_handlers()
{
    return game_actions_uri_handlers_array;
}

size_t game_actions_uri_handlers_size()
{
    size_t n = 0;
#define X(action_name, function, ARGUMENTS, OUTPUT) \
    n++;
DEFINE_GAME_ACTION_FUNCTIONS
#undef X

    return n;
}

