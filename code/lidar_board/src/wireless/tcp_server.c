#include "wireless/tcp_server.h"
#include "system/task_priority.h"
#include "localization/pose.h"
#include "localization/pose_refinement.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>

#include <math.h>

#define TAG "TCP_Server"
#define LIDAR_PORT 11120

struct tcp_query_t {
    int16_t guess_pose_x_mm;
    int16_t guess_pose_y_mm;
    int16_t guess_pose_theta_mrad;
} __attribute__((packed));

struct tcp_response_t {
    int16_t refined_pose_x_mm;
    int16_t refined_pose_y_mm;
    int16_t refined_pose_theta_mrad;
    uint8_t has_refined;
    uint8_t obstacle_clusters[NUMBER_OF_CLUSTER_ANGLES];
} __attribute__((packed));

static int socket_fd;

static void tcp_socket_task(void *args);

void init_tcp_server(void)
{
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        ESP_LOGE(TAG, "Failed to create TCP socket");
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = LIDAR_PORT,
        .sin_addr = {
            .s_addr = htonl(INADDR_ANY)
        }
    };
    if (bind (socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE(TAG, "Failed to bind socket to port %d", LIDAR_PORT);
    }

    listen(socket_fd, 1);

    TaskHandle_t task;
    xTaskCreate(tcp_socket_task, "tcp_socket", TASK_STACK_SIZE, NULL, TCP_SOCKET_PRIORITY, &task);
}

static void tcp_socket_task(void *args)
{
    while (true) {
        int conn_fd = accept(socket_fd, NULL, NULL);
        ESP_LOGD(TAG, "New TCP client connected");

        struct tcp_query_t query;
        read(conn_fd, (void*)&query, sizeof(query));
        ESP_LOGD(TAG, "Got TCP query");

        struct tcp_response_t response;
        pose_t guess_pose, refined_pose;
        guess_pose.x = query.guess_pose_x_mm;
        guess_pose.y = query.guess_pose_y_mm;
        guess_pose.theta = (float)query.guess_pose_theta_mrad / 1000.0;
        ESP_LOGD(TAG, "Estimated pose from main board: %f %f %f", guess_pose.x, guess_pose.y, guess_pose.theta);
        update_estimated_input_pose(&guess_pose);

        response.has_refined = get_refined_output_pose(&refined_pose);
        if (response.has_refined) {
            response.refined_pose_x_mm = refined_pose.x;
            response.refined_pose_y_mm = refined_pose.y;
            response.refined_pose_theta_mrad = refined_pose.theta * 1000.0;
        }
        float obstacle_clusters[NUMBER_OF_CLUSTER_ANGLES];
        get_obstacle_clusters(obstacle_clusters);
        for (int i = 0; i < NUMBER_OF_CLUSTER_ANGLES; ++i) {
            if (isnanf(obstacle_clusters[i]) || isinff(obstacle_clusters[i])) {
                response.obstacle_clusters[i] = 0xFF;
            } else {
                response.obstacle_clusters[i] = (uint8_t)floorf(obstacle_clusters[i] / 10.0);
            }
        }

        size_t s = 0;
        do {
            s += write(conn_fd, &response + s, sizeof(response) - s);
        } while(s < sizeof(response));

        close(conn_fd);
    }
}
