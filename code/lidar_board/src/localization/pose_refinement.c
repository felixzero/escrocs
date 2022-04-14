#include "localization/pose_refinement.h"
#include "localization/least_square_helpers.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <math.h>

#define TAG "Pose refinement"
#define INITIAL_REFLECTIVITY_THRESHOLD 3500000
#define INITIAL_MAX_ACCEPTABLE_BEACON_ERROR 200.0
#define MINIMUM_OBSTACLE_DISTANCE 150.0
#define NUMBER_OF_BEACONS 3

#define INDEX_TO_ANGLE(i, offset) ((float)i / NUMBER_OF_ANGLES * 2.0 * M_PI - (offset))
#define SQUARE(x) ((x) * (x))

static QueueHandle_t input_pose_queue, output_pose_queue, tuning_queue, obstacle_clusters_queue;

static point_t beacon_positions[] = {
    { .x = 0.0, .y = 0.0 },
    { .x = 0.0, .y = -1000.0 },
    { .x = 2000.0, .y = 0.0 }
};

void init_pose_refinement(void)
{
    input_pose_queue = xQueueCreate(1, sizeof(pose_t));
    output_pose_queue = xQueueCreate(1, sizeof(pose_t));
    tuning_queue = xQueueCreate(1, sizeof(pose_refinement_tuning_t));
    obstacle_clusters_queue = xQueueCreate(1, sizeof(float) * NUMBER_OF_CLUSTER_ANGLES);

    pose_refinement_tuning_t initial_tuning = {
        .reflectivity_threshold = INITIAL_REFLECTIVITY_THRESHOLD,
        .max_acceptable_beacon_error = INITIAL_MAX_ACCEPTABLE_BEACON_ERROR,
        .angle_offset = 0.0,
    };
    set_pose_refinement_tuning(&initial_tuning);
}

void update_estimated_input_pose(const pose_t *estimated_pose)
{
    xQueueOverwrite(input_pose_queue, estimated_pose);
}

bool get_refined_output_pose(pose_t *output_pose)
{
    return xQueueReceive(output_pose_queue, output_pose, 0);
}

static int index_of_closest_beacon(const point_t beacon_positions[], point_t point, float max_acceptable_beacon_error)
{
    int result = -1;
    float min_sq_distance_to_beacon = infinityf();
    for (int i = 0; i < NUMBER_OF_BEACONS; ++i) {
        float sq_distance_to_beacon = SQUARE(beacon_positions[i].x - point.x) + SQUARE(beacon_positions[i].y - point.y);
        if (sq_distance_to_beacon < min_sq_distance_to_beacon) {
            result = i;
            min_sq_distance_to_beacon = sq_distance_to_beacon;
        }
    }
    if (min_sq_distance_to_beacon  < SQUARE(max_acceptable_beacon_error)) {
        return result;
    }
    return -1;
}

void refine_pose_from_point_cloud(const uint16_t distances[], const uint16_t intensities[])
{
    pose_t estimated_pose;
    if (!xQueueReceive(input_pose_queue, &estimated_pose, 0)) {
        return;
    }
    pose_refinement_tuning_t tuning;
    if (!xQueuePeek(tuning_queue, &tuning, 0)) {
        ESP_LOGW(TAG, "No tuning parameters given for pose refiner");
        return;
    }

    point_t measured_beacon_positions[] = {
        { .x = 0.0, .y = 0.0 },
        { .x = 0.0, .y = 0.0 },
        { .x = 0.0, .y = 0.0 },
    };
    int number_of_points_per_beacon[] = { 0, 0, 0 };

    // Loop over all points
    int number_of_rejects = 0;
    for (int i = 0; i < NUMBER_OF_ANGLES; ++i) {
        // Filter out points of low reflectivity
        float reflectivity = (float)intensities[i] * distances[i];
        if (reflectivity < tuning.reflectivity_threshold) {
            continue;
        }

        // Check which beacon is the closest, ignoring potential false positives
        point_t estimated_point = {
            .x = - (distances[i] * cosf(INDEX_TO_ANGLE(i, tuning.angle_offset) - estimated_pose.theta) - estimated_pose.x),
            .y = distances[i] * sinf(INDEX_TO_ANGLE(i, tuning.angle_offset) - estimated_pose.theta) + estimated_pose.y,
        };
        int index = index_of_closest_beacon(beacon_positions, estimated_point, tuning.max_acceptable_beacon_error);
        if (index == -1) {
            number_of_rejects++;
            continue;
        }

        // Add points to mean position
        measured_beacon_positions[index].x += - distances[i] * cosf(INDEX_TO_ANGLE(i, tuning.angle_offset));
        measured_beacon_positions[index].y += distances[i] * sinf(INDEX_TO_ANGLE(i, tuning.angle_offset));
        number_of_points_per_beacon[index]++;
    }

    // Remove undetected beacons and compute average
    point_t actual_positions[NUMBER_OF_BEACONS];
    point_t relative_positions[NUMBER_OF_BEACONS];
    int number_of_identified_beacons = 0;
    for (int i = 0; i < NUMBER_OF_BEACONS; ++i) {
        if (number_of_points_per_beacon[i] == 0) {
            continue;
        }
        actual_positions[number_of_identified_beacons] = beacon_positions[i];
        relative_positions[number_of_identified_beacons] = measured_beacon_positions[i];
        relative_positions[number_of_identified_beacons].x /= number_of_points_per_beacon[i];
        relative_positions[number_of_identified_beacons].y /= number_of_points_per_beacon[i];
        number_of_identified_beacons++;
    }

    ESP_LOGI(TAG, "Refining pose with %d identified beacons - %d rejects", number_of_identified_beacons, number_of_rejects);
    pose_t refined_pose = find_pose_from_beacons(actual_positions, relative_positions, number_of_identified_beacons, estimated_pose);
    ESP_LOGI(TAG, "Pose refined to %f %f %f", refined_pose.x, refined_pose.y, refined_pose.theta);
    xQueueOverwrite(output_pose_queue, &refined_pose);
}

void set_beacon_position(unsigned int beacon_index, float x, float y)
{
    assert(beacon_index < sizeof(beacon_positions) / sizeof(beacon_positions[0]));
    beacon_positions[beacon_index].x = x;
    beacon_positions[beacon_index].y = y;
}

void set_pose_refinement_tuning(const pose_refinement_tuning_t *tuning)
{
    xQueueOverwrite(tuning_queue, tuning);
}

void compute_obstacle_clusters(const uint16_t point_cloud[])
{
    float distances_by_angle[NUMBER_OF_CLUSTER_ANGLES];

    pose_refinement_tuning_t tuning;
    if (!xQueuePeek(tuning_queue, &tuning, 0)) {
        ESP_LOGW(TAG, "No tuning parameters given for pose refiner");
        return;
    }

    for (int i = 0; i < NUMBER_OF_CLUSTER_ANGLES; ++i) {
        distances_by_angle[i] = INFINITY;
    }

    for (int i = 0; i < NUMBER_OF_ANGLES; ++i) {
        float angle = INDEX_TO_ANGLE(i, tuning.angle_offset);

        int angle_index = floorf(fmodf(angle / 2.0 / M_PI * NUMBER_OF_CLUSTER_ANGLES, NUMBER_OF_CLUSTER_ANGLES));
        if (point_cloud[i] > MINIMUM_OBSTACLE_DISTANCE) {
            distances_by_angle[angle_index] = fminf(point_cloud[i], distances_by_angle[angle_index]);
        }
    }

    xQueueOverwrite(obstacle_clusters_queue, distances_by_angle);
}

void get_obstacle_clusters(float *distances_by_angle)
{
    xQueuePeek(obstacle_clusters_queue, distances_by_angle, 0);
}
