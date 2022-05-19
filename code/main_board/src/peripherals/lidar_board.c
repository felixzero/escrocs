#include "peripherals/lidar_board.h"

#include <esp_err.h>
#include <esp_log.h>
#include <math.h>

#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>

#define TAG "Lidar"
#define LIDAR_BOARD_IP_ADDRESS "192.168.4.2"

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

bool refine_pose(const pose_t *guess_pose, pose_t *refined_pose, float *obstacle_distances_by_angle)
{
    struct timeval socket_timeout = {
        .tv_sec = 0,
        .tv_usec = 5000
    };

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        ESP_LOGE(TAG, "Cannot create TCP socket");
        return false;
    }

    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = 11120,
        .sin_addr = {}
    };
    inet_pton(AF_INET, LIDAR_BOARD_IP_ADDRESS, &serv_addr.sin_addr);

    setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&socket_timeout, sizeof(socket_timeout));
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&socket_timeout, sizeof(socket_timeout));

    if (connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        ESP_LOGE(TAG, "Cannot connect to lidar board");
        close(socket_fd);
        return false;
    }

    struct tcp_query_t query;
    write(socket_fd, &query, sizeof(query));
    struct tcp_response_t response;
    read(socket_fd, &response, sizeof(response));

    for (int i = 0; i < NUMBER_OF_CLUSTER_ANGLES; ++i) {
        obstacle_distances_by_angle[i] = (float)response.obstacle_clusters[i];
    }

    if (response.has_refined) {
        refined_pose->x = response.refined_pose_x_mm;
        refined_pose->y = response.refined_pose_y_mm;
        refined_pose->theta = (float)response.refined_pose_theta_mrad / 1000.0;

        ESP_LOGD(
            TAG, "Lidar board refined position from (%f, %f, %f) to (%f, %f, %f)",
            guess_pose->x, guess_pose->y, guess_pose->theta,
            refined_pose->x, refined_pose->y, refined_pose->theta
        );
    } else {
        *refined_pose = *guess_pose;
    }

    close(socket_fd);

    return true;
}

