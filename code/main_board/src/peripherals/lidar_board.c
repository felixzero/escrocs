#include "peripherals/lidar_board.h"

#include <esp_err.h>
#include <esp_log.h>
#include <math.h>

#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>

#define TAG "Lidar"
#define LIDAR_BOARD_IP_ADDRESS 2

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
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        ESP_LOGE(TAG, "Cannot create TCP socket");
        return false;
    }

    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = 11120,
        .sin_addr = {
            .s_addr = PP_HTONL(LWIP_MAKEU32(192, 168, CONFIG_ESP_WIFI_SUBNET, LIDAR_BOARD_IP_ADDRESS))
        }
    };

    if (connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        ESP_LOGE(TAG, "Cannot connect to lidar board");
        close(socket_fd);
        return false;
    }

    struct tcp_query_t query;
    query.guess_pose_x_mm = (int)guess_pose->x;
    query.guess_pose_y_mm = (int)guess_pose->y;
    query.guess_pose_theta_mrad = (int)(guess_pose->theta * 1000);

    size_t s = 0;
    do {
        s += write(socket_fd, &query + s, sizeof(query) - s);
    } while (s < sizeof(query));
    struct tcp_response_t response;
    if(read(socket_fd, &response, sizeof(response)) < sizeof(response)) {
        ESP_LOGW(TAG, "Timeout while receiving lidar data");
        return false;
    }

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
