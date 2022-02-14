#include "wireless/httpd.h"
#include "system/task_priority.h"
#include "wireless/ota.h"

#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <esp_log.h>
#include <mbedtls/base64.h>

#define TAG "httpd"

struct socket_fd_list {
    int fd;
    struct socket_fd_list *next;
};

struct async_send_args {
    char *buffer;
    size_t length;
};

static struct socket_fd_list *opened_socket_fd = NULL;
static httpd_handle_t server_handle = NULL;
static esp_err_t server_sent_events_handler(httpd_req_t *req);
static void async_send_over_sse(void *args);


static httpd_uri_t uri_handlers[] = {
    {
        .uri = "/firmware.bin",
        .method = HTTP_PUT,
        .handler = firmware_upgrade_handler,
        .user_ctx = NULL,
    },
    {
        .uri = "/events",
        .method = HTTP_GET,
        .handler = server_sent_events_handler,
        .user_ctx = NULL
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

esp_err_t send_to_clients(uint8_t *buffer, size_t length)
{
    struct async_send_args *args = malloc(sizeof(struct async_send_args));
    const char prefix_str[] = "data: ";
    const char suffix_str[] = "\r\n\r\n";

    size_t encoded_length;
    mbedtls_base64_encode(NULL, 0, &encoded_length, buffer, length);
    args->buffer = malloc(encoded_length + sizeof(prefix_str) + sizeof(suffix_str) - 1);
    memcpy(args->buffer, prefix_str, sizeof(prefix_str) - 1);
    mbedtls_base64_encode(
        (unsigned char*)args->buffer + sizeof(prefix_str) - 1,
        encoded_length,
        &encoded_length,
        buffer,
        length
    );
    strcpy(args->buffer + sizeof(prefix_str) - 1 + encoded_length, suffix_str);
    args->length = encoded_length + sizeof(prefix_str) + sizeof(suffix_str) - 2;
    return httpd_queue_work(server_handle, async_send_over_sse, args);
}

static esp_err_t server_sent_events_handler(httpd_req_t *req)
{
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

    const char header_str[] = "HTTP/1.1 200 OK\r\nContent-Type: text/event-stream\r\n\r\n";
    httpd_socket_send(server_handle, current_client_fd, header_str, strlen(header_str), 0);
    
    return ESP_OK;
}

static void async_send_over_sse(void *args)
{
    struct async_send_args *send_args = (struct async_send_args*)args;
    struct socket_fd_list **fd_list_iterator = &opened_socket_fd;
    while (*fd_list_iterator != NULL) {
        ESP_LOGI(TAG, "Sending server sent event to client %d", (*fd_list_iterator)->fd);
        esp_err_t error = httpd_socket_send(
            server_handle,
            (*fd_list_iterator)->fd,
            send_args->buffer,
            send_args->length,
            0
        );

        if (error == ESP_ERR_INVALID_ARG) {
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
