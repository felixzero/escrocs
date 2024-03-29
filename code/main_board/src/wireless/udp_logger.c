#include "wireless/udp_logger.h"
#include "system/task_priority.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>

#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>

#define TAG "UDP_Logger"
#define DEBUG_PORT 11111

#define MAX_MESSAGE_LENGTH 128
#define MAX_MESSAGE_NUMBER 32

static QueueHandle_t network_log_queue;

static int socket_fd;
static struct sockaddr_in server_addr;

static int logger_function(const char *message, va_list args);
static void network_logger_task(void *args);

esp_err_t init_udp_logger(void)
{
    ESP_LOGI(TAG, "Initializing UDP logger");

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        ESP_LOGW(TAG, "Failed to create UDP logger socket");
        return ESP_FAIL;
    }

    int broadcast_enable = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0) {
        ESP_LOGW(TAG, "Cannot create broadcast socket");
        return ESP_FAIL;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DEBUG_PORT);
    server_addr.sin_addr.s_addr = PP_HTONL(LWIP_MAKEU32(192, 168, CONFIG_ESP_WIFI_SUBNET, 255));

    TaskHandle_t task;
    network_log_queue = xQueueCreate(MAX_MESSAGE_NUMBER, sizeof(char*));
    xTaskCreatePinnedToCore(network_logger_task, "network_logger", TASK_STACK_SIZE, NULL, LOGGER_PRIORITY, &task, LOW_CRITICITY_CORE);

    esp_log_set_vprintf(logger_function);

    return ESP_OK;
}

static int logger_function(const char *fmt, va_list args)
{
    char *message = malloc(MAX_MESSAGE_LENGTH);
    int length = vsnprintf(message, MAX_MESSAGE_LENGTH, fmt, args);

    // First print to serial port
    puts(message);

    // Then put message into network queue
    xQueueSend(network_log_queue, &message, 0);

    return length;
}

static void network_logger_task(void *args)
{
    while (true) {
        char *message;
        if (!xQueueReceive(network_log_queue, &message, 100)) {
            continue;
        }

        size_t message_length = strlen(message);
        if (message[message_length - 1] != '\n') {
            message[message_length - 1] = '\n';
        }

        sendto(socket_fd, message, strlen(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        free(message);
    }
}
