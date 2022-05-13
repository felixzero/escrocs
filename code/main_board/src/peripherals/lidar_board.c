#include "peripherals/lidar_board.h"
#include "system/i2c_master.h"

#include <esp_err.h>
#include <esp_log.h>
#include <math.h>

#define TAG "Lidar"
#define LIDAR_I2C_PORT I2C_PORT_MOTOR
#define LIDAR_I2C_ADDR 0x0D
#define MAX_VALID_DIMENSION 10000.0
#define HEADER_VALUE 0xBC

struct i2c_query_t {
    uint8_t header;
    int16_t guess_pose_x_mm;
    int16_t guess_pose_y_mm;
    int16_t guess_pose_theta_mrad;
    uint8_t checksum;
} __attribute__((packed));

struct i2c_response_t {
    uint8_t header;
    int16_t refined_pose_x_mm;
    int16_t refined_pose_y_mm;
    int16_t refined_pose_theta_mrad;
    uint8_t has_refined;
    uint8_t obstacle_clusters[NUMBER_OF_CLUSTER_ANGLES];
    uint8_t checksum;
} __attribute__((packed));

static uint8_t compute_checksum(uint8_t *payload, size_t length);

void refine_pose(const pose_t *guess_pose, pose_t *refined_pose, float *obstacle_distances_by_angle)
{
    struct i2c_query_t query;
    struct i2c_response_t response;

    query.header = HEADER_VALUE;
    query.guess_pose_x_mm = guess_pose->x;
    query.guess_pose_y_mm = guess_pose->y;
    query.guess_pose_theta_mrad = guess_pose->theta * 1000.0;
    query.checksum = compute_checksum((uint8_t*)&query, sizeof(query));

    send_to_i2c(LIDAR_I2C_PORT, LIDAR_I2C_ADDR, &query, sizeof(struct i2c_query_t));
    request_from_i2c(LIDAR_I2C_PORT, LIDAR_I2C_ADDR, &response, sizeof(struct i2c_response_t));

    *refined_pose = *guess_pose;
    if ((compute_checksum((uint8_t*)&response, sizeof(response)) != response.checksum) || (response.header != HEADER_VALUE)) {
        ESP_LOGW(TAG, "Warning: bad checksum on response from lidar board");
        return;
    }

    for (int i = 0; i < NUMBER_OF_CLUSTER_ANGLES; ++i) {
        obstacle_distances_by_angle[i] = (float)response.obstacle_clusters[i];
    }

    if (response.has_refined) {
        refined_pose->x = response.refined_pose_x_mm;
        refined_pose->y = response.refined_pose_y_mm;
        refined_pose->theta = (float)response.refined_pose_theta_mrad / 1000.0;

        ESP_LOGI(
            TAG, "Lidar board refined position from (%f, %f, %f) to (%f, %f, %f)",
            guess_pose->x, guess_pose->y, guess_pose->theta,
            refined_pose->x, refined_pose->y, refined_pose->theta
        );
    }
}

static uint8_t compute_checksum(uint8_t *payload, size_t length)
{
    uint8_t result = 0x00;
    for (size_t i = 0; i < length - 1; ++i) {
        result ^= payload[i];
    }
    return result;
}
