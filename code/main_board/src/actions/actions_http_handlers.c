#include "actions/game_actions.h"

#include <esp_err.h>
#include <esp_log.h>
#include <esp_http_server.h>
#include <cJSON.h>

#define TAG "httpd"

#define MAX_BUFFER_SIZE 4096
#define QUERY_PARAM_MAX_LENGTH 32
static char buffer[MAX_BUFFER_SIZE];

static int get_channel_from_query_param(httpd_req_t *req);

// Define PUT handlers
#define X(variable_name, setter_function, getter_function, DEFINE_GAME_ACTIONS_DATA) \
static esp_err_t put_ ## variable_name ## _http_handler(httpd_req_t *req) \
{   \
    ESP_LOGI(TAG, "Received request to PUT /" #variable_name); \
    int length_read = httpd_req_recv(req, buffer, sizeof(buffer)); \
    if (length_read != req->content_len) { \
        ESP_LOGI(TAG, "Error while handling PUT /" #variable_name); \
        return ESP_FAIL; \
    } \
    \
    cJSON *root = cJSON_ParseWithLength(buffer, length_read); \
    if (root == NULL) { \
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON input\n"); \
        return ESP_OK; \
    } \
    \
    cJSON *item; \
    struct GAME_ACTION_STRUCT_NAME(variable_name) data; \
    DEFINE_GAME_ACTIONS_DATA \
    int result = setter_function(get_channel_from_query_param(req), data); \
    \
    if (result == 0) { \
        httpd_resp_send(req, "OK\n", HTTPD_RESP_USE_STRLEN); \
    } else { \
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed\n"); \
    } \
    return ESP_OK; \
}

#define X_FLOAT_DATA(parameter_name) \
item = cJSON_GetObjectItem(root, #parameter_name); \
if (item == NULL || !cJSON_IsNumber(item)) { \
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Could not retrieve parameter " #parameter_name "\n"); \
    return ESP_OK; \
} \
data.parameter_name = cJSON_GetNumberValue(item);

#define X_INT_DATA(parameter_name) \
item = cJSON_GetObjectItem(root, #parameter_name); \
if (item == NULL || !cJSON_IsNumber(item)) { \
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Could not retrieve parameter " #parameter_name "\n"); \
    return ESP_OK; \
} \
data.parameter_name = cJSON_GetNumberValue(item);

#define X_BOOL_DATA(parameter_name) \
item = cJSON_GetObjectItem(root, #parameter_name); \
if (item == NULL || !cJSON_IsBool(item)) { \
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Could not retrieve parameter " #parameter_name "\n"); \
    return ESP_OK; \
} \
data.parameter_name = cJSON_IsTrue(item);

DEFINE_GAME_VARIABLES
#undef X
#undef X_FLOAT_DATA
#undef X_INT_DATA
#undef X_BOOL_DATA

// Define GET handlers

#define X(variable_name, setter_function, getter_function, DEFINE_GAME_ACTIONS_DATA) \
static esp_err_t get_ ## variable_name ## _http_handler(httpd_req_t *req) \
{   \
    ESP_LOGI(TAG, "Received request to GET /" #variable_name); \
    struct GAME_ACTION_STRUCT_NAME(variable_name) data; \
    int result = getter_function(get_channel_from_query_param(req), &data); \
    cJSON *root = cJSON_CreateObject(); \
    cJSON *entry; \
    DEFINE_GAME_ACTIONS_DATA \
    char *json_string = cJSON_Print(root); \
    \
    if (result == 0) { \
        httpd_resp_send(req, json_string, HTTPD_RESP_USE_STRLEN); \
    } else { \
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed\n"); \
    } \
    return ESP_OK; \
}

#define X_FLOAT_DATA(parameter_name) \
entry = cJSON_CreateNumber(data.parameter_name); \
cJSON_AddItemToObject(root, #parameter_name, entry);

#define X_INT_DATA(parameter_name) \
entry = cJSON_CreateNumber(data.parameter_name); \
cJSON_AddItemToObject(root, #parameter_name, entry);

#define X_BOOL_DATA(parameter_name) \
entry = cJSON_CreateBool(data.parameter_name); \
cJSON_AddItemToObject(root, #parameter_name, entry);

DEFINE_GAME_VARIABLES
#undef X
#undef X_FLOAT_DATA
#undef X_INT_DATA
#undef X_BOOL_DATA

// Define URI handler list

#define X(variable_name, setter_function, getter_function, DEFINE_GAME_ACTIONS_DATA) \
    { \
        .uri = "/" # variable_name, \
        .method = HTTP_PUT, \
        .handler = put_ ## variable_name ## _http_handler, \
        .user_ctx = NULL, \
    }, \
    { \
        .uri = "/" # variable_name, \
        .method = HTTP_GET, \
        .handler = get_ ## variable_name ## _http_handler, \
        .user_ctx = NULL, \
    },

httpd_uri_t game_actions_uri_handlers_array[] = {
DEFINE_GAME_VARIABLES
};
#undef X

httpd_uri_t *get_game_actions_uri_handlers()
{
    return game_actions_uri_handlers_array;
}

size_t game_actions_uri_handlers_size()
{
    size_t n = 0;
#define X(variable_name, setter_function, getter_function, DEFINE_GAME_ACTIONS_DATA) \
    n += 2;
DEFINE_GAME_VARIABLES
#undef X

    return n;
}

static int get_channel_from_query_param(httpd_req_t *req)
{
    char buffer[QUERY_PARAM_MAX_LENGTH], id_str[QUERY_PARAM_MAX_LENGTH];
    httpd_req_get_url_query_str(req, buffer, QUERY_PARAM_MAX_LENGTH);
    if (httpd_query_key_value(buffer, "channel", id_str, QUERY_PARAM_MAX_LENGTH) == ESP_ERR_NOT_FOUND) {
        return 0;
    }
    return atoi(id_str);
}
