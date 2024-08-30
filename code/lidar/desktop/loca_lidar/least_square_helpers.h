#pragma once

#include "amalgame.h"

static void least_square_fit_two_beacons(point_t actual_positions[], point_t relative_positions[], pose_t *result);
static void least_square_fit_three_beacons(point_t actual_positions[], point_t relative_positions[], pose_t *result);
pose_t find_pose_from_beacons(point_t actual_positions[], point_t relative_positions[], size_t number_of_beacons, pose_t estimated_pose);
