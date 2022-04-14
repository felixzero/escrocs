#pragma once

#include "localization/pose.h"

#include <stdbool.h>

typedef struct {
    float reflectivity_threshold;
    float max_acceptable_beacon_error;
    float angle_offset;
} pose_refinement_tuning_t;

/**
 * Init pose refinement subsystem
 */
void init_pose_refinement(void);

/**
 * Updated the input pose with latest estimated guess
 */
void update_estimated_input_pose(const pose_t *estimated_pose);

/**
 * Get the latest refined pose
 * @param output_pose pointer to store result
 * @return true if a refined pose has been calculated, false otherwise
 */
bool get_refined_output_pose(pose_t *output_pose);

/**
 * Compute the pose of the lidar from an initial estimated pose and a point cloud
 */
void refine_pose_from_point_cloud(const uint16_t distances[], const uint16_t intensities[]);

/**
 * Declare the theoretical position of the beacons
 */
void set_beacon_position(unsigned int beacon_index, float x, float y);

/**
 * Set the tuning parameters of the refinement algorithm
 */
void set_pose_refinement_tuning(const pose_refinement_tuning_t *tuning);

/**
 * Number of obstacle sectors to perform detection on
 */
#define NUMBER_OF_CLUSTER_ANGLES 8

/**
 * From the point cloud, compute the nearest obstacle distance by sectors
 */
void compute_obstacle_clusters(const uint16_t point_cloud[]);

/**
 * Get the latest obstacle distances
 */
void get_obstacle_clusters(float *distances_by_angle);