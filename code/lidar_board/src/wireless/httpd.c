#include "wireless/httpd.h"
#include "system/task_priority.h"
#include "wireless/ota.h"

#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <esp_log.h>

#define TAG "httpd"

struct socket_fd_list {
    int fd;
    struct socket_fd_list *next;
};

struct async_websocket_send_args {
    uint8_t *buffer;
    size_t length;
};

static struct socket_fd_list *opened_socket_fd = NULL;
static httpd_handle_t server_handle = NULL;
static esp_err_t websocket_handler(httpd_req_t *req);
static void async_send_over_websocket(void *args);


static httpd_uri_t uri_handlers[] = {
    {
        .uri = "/firmware.bin",
        .method = HTTP_PUT,
        .handler = firmware_upgrade_handler,
        .user_ctx = NULL,
    },
    {
        .uri = "/points",
        .method = HTTP_GET,
        .handler = websocket_handler,
        .user_ctx = NULL,
        .is_websocket = true,
        .handle_ws_control_frames = false
    }
};

#define URI_HANDLERS_LENGTH (sizeof(uri_handlers) / sizeof(uri_handlers[0]))

void init_http_server(void)
{
    opened_socket_fd = NULL;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    if (httpd_start(&server_handle, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        vTaskDelete(NULL);
    }

    for (int i = 0; i < URI_HANDLERS_LENGTH; ++i) {
        httpd_register_uri_handler(server_handle, &uri_handlers[i]);
    }

    ESP_LOGI(TAG, "HTTP server properly initiated");

    init_ota_rebooter();
}

esp_err_t send_over_websocket(uint8_t *buffer, size_t length)
{
    struct async_websocket_send_args *args = malloc(sizeof(struct async_websocket_send_args));
    args->buffer = malloc(length);
    memcpy(args->buffer, buffer, length);
    args->length = length;
    return httpd_queue_work(server_handle, async_send_over_websocket, args);
    return ESP_OK;
}

static void async_send_over_websocket(void *args)
{
    struct async_websocket_send_args *send_args = (struct async_websocket_send_args*)args;
    struct socket_fd_list **fd_list_iterator = &opened_socket_fd;
    while (*fd_list_iterator != NULL) {
        ESP_LOGI(TAG, "Sending WebSocket frame to client %d", (*fd_list_iterator)->fd);
        httpd_ws_frame_t ws_pkt = {
            .type = HTTPD_WS_TYPE_BINARY,
            .payload = send_args->buffer,
            .len = send_args->length
        };
        esp_err_t error = httpd_ws_send_frame_async(server_handle, (*fd_list_iterator)->fd, &ws_pkt);
        if (ESP_ERROR_CHECK_WITHOUT_ABORT(error) != ESP_OK) {
            ESP_LOGW(TAG, "Dropping client %d", (*fd_list_iterator)->fd);
            httpd_sess_trigger_close(server_handle, (*fd_list_iterator)->fd);
            struct socket_fd_list *current_item = *fd_list_iterator;
            *fd_list_iterator = (*fd_list_iterator)->next;
            free(current_item);
        } else {
            fd_list_iterator = &((*fd_list_iterator)->next);
        }
    }

    free(send_args->buffer);
    free(send_args);
}

static esp_err_t websocket_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }

    int current_client_fd = httpd_req_to_sockfd(req);
    struct socket_fd_list *fd_iterator = opened_socket_fd;
    while (fd_iterator != NULL) {
        if (fd_iterator->fd == current_client_fd) {
            ESP_LOGI(TAG, "Current client already known; dropping");
            return ESP_OK;
        }
        fd_iterator = fd_iterator->next;
    }

    ESP_LOGI(TAG, "New client connected: %d", current_client_fd);
    struct socket_fd_list *new_client = malloc(sizeof(struct socket_fd_list));
    new_client->fd = current_client_fd;
    new_client->next = opened_socket_fd;
    opened_socket_fd = new_client;
    return ESP_OK;
}
