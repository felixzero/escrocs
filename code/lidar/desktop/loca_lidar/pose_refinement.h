#pragma once

#include <stdint.h>
#include "amalgame.h"
#include "least_square_helpers.h"

#define MAX_NB_BEACONS 3
#define SQUARE(x) ((x) * (x))

typedef struct {
    int32_t max_sq_dist_expected; //max dist expected between amalgame & beacon
} pose_tuning_t;

extern pose_t estimated_lidar;
extern pose_t refined_lidar;
extern uint16_t last_assos[MAX_NB_BEACONS]; //index of last_asso = beacon, content = amalgame


static point_t beacon_positions[3] = {
    {50, -94}, 
    {1500, 2094},
    {2950, -94}
};

pose_t get_refined_lidar();
void set_estimated_pose(int32_t x, int32_t y, float angle);
void convert_xy(point_t* pts, uint16_t count, const uint16_t angles[], const uint16_t distances[]);
pose_t refine_pose(point_t* candidates, amalgame_t* amalgames, uint16_t nb_candidates, 
    const pose_tuning_t* tuning);
static void calc_expected_beacon_pos(point_t* expected_positions);
static point_t refine_pos_beacon(uint16_t* angles, uint16_t* distances);
static int8_t index_closest_amalg(point_t expected, const point_t candidates[], size_t nb_candidates, 
    int32_t max_sq_dist);



