#pragma once

#include <stdint.h>
#include "amalgame.h"
#include "least_square_helpers.h"

#define MAX_NB_BEACONS 3
#define MAX_CANDIDATES_BEACONS 8 //Careful : last index is kept as 255 for further processing
#define POW_CANDIDATE_BEACONS 25 //MAX_CANDIDATES_BEACONS^MAX_NB_BEACONS = 5^(3-1)
#define SQUARE(x) ((x) * (x))

typedef struct {
    int32_t max_sq_dist_expected; //max dist expected between amalgame & beacon
    int32_t max_dist_beacons; //max dist difference between expected distances of two beacons
    int32_t max_sq_dist_large_expected; //max dist between amalgame & beacon, with larger area (in case of not reliable odometry)
} pose_tuning_t;

typedef struct {
    uint8_t nb_beacons;
    uint8_t index[MAX_NB_BEACONS];
} valid_combination_t;

extern pose_t estimated_lidar;
extern pose_t refined_lidar;
extern uint16_t last_assos[MAX_NB_BEACONS]; //index of last_asso = beacon, content = amalgame
extern uint8_t indexs_debug[MAX_CANDIDATES_BEACONS];

extern pose_t too_many_corr_pose;
extern pose_t too_many_candidates_pose;

static point_t beacon_positions[3] = {
    {3094, 50},
    {-94, 1000},
    {3094, 1950}
};
    // ENAC 2022 :
    //{50, -94}, 
    //{1500, 2094},
    //{2950, -94}

extern uint32_t distances_beacons[MAX_NB_BEACONS][MAX_NB_BEACONS];
pose_t get_refined_lidar();
void set_estimated_pose(int32_t x, int32_t y, float angle);
void convert_xy(point_t* pts, uint16_t count, const uint16_t angles[], const uint16_t distances[]);
pose_t refine_pose(point_t* candidates, amalgame_t* amalgames, uint16_t nb_amalgs, 
    pose_t estimated_odom, const pose_tuning_t* tuning);
static void calc_expected_beacon_pos(pose_t estimated_pose, point_t* expected_positions);
static point_t refine_pos_beacon(uint16_t* angles, uint16_t* distances);
static uint8_t find_correspondance(uint8_t* correspondances, uint8_t indexs[MAX_NB_BEACONS][MAX_CANDIDATES_BEACONS], 
            const point_t candidates[], size_t nb_candidates, pose_tuning_t* tuning);
static int8_t index_closest_amalg(point_t expected, const point_t candidates[], size_t nb_candidates, 
    int32_t max_sq_dist);
static uint8_t indexs_closest_amalg(uint8_t indexs[MAX_CANDIDATES_BEACONS], point_t expected, const point_t candidates[], 
    size_t nb_candidates, int32_t max_sq_dist);
static size_t generate_combination(valid_combination_t* combinations, uint16_t max_combinations, 
    const uint8_t indexs[MAX_NB_BEACONS][MAX_CANDIDATES_BEACONS], const point_t candidates[], uint32_t max_dist);
static uint8_t update_valid_combination(valid_combination_t* valid_combination, valid_combination_t combination_add);



