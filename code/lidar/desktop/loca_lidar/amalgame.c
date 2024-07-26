#include "amalgame.h"

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

static raw_lidar_t empty_lidar = {
    .count = 0,
    .angles = NULL,
    .distances = NULL,
    .intensities = NULL
};

// returns count of amalgame, and return amalgame points through amalgames_out
//TODO : handles too many pt per amalg, & too many amalgames
int calc_amalgames(amalgame_finder_tuning_t tuning, raw_lidar_t data, raw_lidar_t *amalgames_out) {
    uint16_t last_dist = 0, amalgs_i = 0;
    amalgames_out = (raw_lidar_t*) malloc(sizeof(raw_lidar_t) * tuning.max_amalg_count);
    raw_lidar_t cur_amalg;
    reset_raw_lidar(&cur_amalg, tuning.max_pt_per_amalg, 0);

    for (uint16_t i = 0; i < data.count; i++)
    {
        uint16_t cur_dist = data.distances[i];
        //if outside nominal point range, outside intensity range, distance too high with previous point
        if(cur_dist < tuning.min_dist || cur_dist > tuning.max_dist
        || (data.intensities[i] < tuning.min_intensity)
        || (last_dist != 0 && abs(last_dist - cur_dist) > tuning.max_distance_betwn_pts)
        ) {
            if(last_dist != 0 && cur_amalg.count >= 2) {  //exclude too small amalgame & reset it
                amalgames_out[amalgs_i++] = cur_amalg;
            }
            last_dist = 0;
            continue;
        }
        if(last_dist == 0) {
            reset_raw_lidar(&cur_amalg, tuning.max_pt_per_amalg, 1);
        }

        cur_amalg.angles[cur_amalg.count] = data.angles[i];
        cur_amalg.distances[cur_amalg.count] = data.distances[i];
        cur_amalg.intensities[cur_amalg.count] = data.intensities[i];
        cur_amalg.count++;

        last_dist = data.distances[i];
    }

    //wrap up last amalgame using if needed first amalgame
    if(last_dist != 0) {
        uint16_t first_dist = amalgames_out[0].distances[0];
        if(amalgs_i > 1 && abs(last_dist - first_dist) <= tuning.max_distance_betwn_pts) {
            combine_amalg(&amalgames_out[0], &cur_amalg, tuning.max_pt_per_amalg);
        }
        else {
            amalgames_out[amalgs_i++] = cur_amalg;
        }
    }

    return amalgs_i; // count of amalgames in amalgames_out
    
}

int get_amalg_center(raw_lidar_t* centers_out, raw_lidar_t* amalgames) {

}

static int8_t combine_amalg(raw_lidar_t* dest, raw_lidar_t* add, uint16_t max_count) {
    if(dest->count + add-> count > max_count) {
        return -2; //Error of too many points when combining amalgame
    }
    for (uint16_t i = 0; i < add->count; i++)
    {
        uint16_t j = dest->count;
        dest->angles[i+j] = add->angles[i];
        dest->distances[i+j] = add->distances[i];
        dest->intensities[i+j] = add->intensities[i];
    }
}

static void reset_raw_lidar(raw_lidar_t *item, uint8_t nb_pts, uint8_t need_free) {
    if(need_free) {
        free((void *)item->angles);
        free((void *)item->distances);
        free((void *)item->intensities);
    }

    // Set the structure fields to 0 or NULL
    memset(item, 0, sizeof(raw_lidar_t));

    item->count = 0;
    item->angles = (uint16_t *)calloc(nb_pts, sizeof(uint16_t));    // Allocate and initialize to zero
    item->distances = (uint16_t *)calloc(nb_pts, sizeof(uint16_t)); // Allocate and initialize to zero
    item->intensities = (uint8_t *)calloc(nb_pts, sizeof(uint8_t)); 

}