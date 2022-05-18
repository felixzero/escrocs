#pragma once

#include "motion/motion_control.h"

#define NUMBER_OF_CLUSTER_ANGLES 8

void refine_pose(const pose_t *guess_pose, pose_t *refined_pose, float *obstacle_distances_by_angle);
