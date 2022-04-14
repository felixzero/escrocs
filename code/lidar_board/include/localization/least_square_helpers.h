#pragma once

#include <stddef.h>
#include "localization/pose.h"

/**
 * Calculate the pose of the lidar from the measured position of beacons whose positions are knows
 * @param actual_positions Array of the known positions of the beacons (in the frame of the game table)
 * @param relative_positions Array of the measured positions of the beacons (in the frame of the lidar)
 * @param number_of_beacons Length of both arrays
 * @param estimated_pose Initial guessed value for the pose
 */
pose_t find_pose_from_beacons(point_t actual_positions[], point_t relative_positions[], size_t number_of_beacons, pose_t estimated_pose);
