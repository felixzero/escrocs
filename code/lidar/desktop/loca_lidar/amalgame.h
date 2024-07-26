#pragma once

#include <string.h>
#include <stdint.h>

typedef struct {//min quality of lidar pt
    uint16_t max_distance_betwn_pts;
    uint16_t min_dist; //min dist from lidar to be considered
    uint16_t max_dist; //max dist from lidar ...
    uint8_t min_intensity; 

    uint8_t max_amalg_count;
    uint8_t max_pt_per_amalg;

    
} amalgame_finder_tuning_t;

typedef struct {
    uint16_t count;
    uint16_t *angles;
    uint16_t *distances; 
    uint8_t *intensities; 
} raw_lidar_t;

//TODO : check if needed
typedef struct {
    raw_lidar_t* pts;
    uint16_t avg_angle;
    uint16_t avg_dist;
} amalgame_t;

int calc_amalgames(amalgame_finder_tuning_t tuning, raw_lidar_t data, raw_lidar_t *amalgames_out);
int get_amalg_center(raw_lidar_t* centers_out, raw_lidar_t* amalgames);
static void reset_raw_lidar(raw_lidar_t *item, uint8_t nb_pts, uint8_t need_free);
static int8_t combine_amalg(raw_lidar_t* dest, raw_lidar_t* add, uint16_t max_count);



