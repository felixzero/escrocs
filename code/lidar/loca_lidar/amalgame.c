#include "amalgame.h"

#include <stdint.h>
#include <stdlib.h>
#include <math.h>


#define MIN_FAR_AMALG_COUNT 1
#define MIN_CLOSE_AMALG_COUNT 4
#define FAR_DIST_MM 1000
#define OFFSET_AMALG_CENTER 40 //mm //Used to better approximate center of beacon by adding distance, should be radius of beacon

static raw_lidar_t empty_lidar = {
    .count = 0,
    .angles = NULL,
    .distances = NULL,
    .intensities = NULL
};

static uint16_t abs_dist_mm(polar_t a, polar_t b) {
    float r1 = (float) a.distance;
    float r2 = (float) b.distance;
    float theta1 = a.angle  * (M_PI / 18000.0);
    float theta2 = b.angle  * (M_PI / 18000.0);

    float delta_theta = theta2 - theta1;
    float cos_delta_theta = cos(delta_theta);
    
    return sqrt(r1 * r1 + r2 * r2 - 2 * r1 * r2 * cos_delta_theta);
}

void init_amalgames(amalgame_finder_tuning_t tuning, amalgame_t* amalgames) {
    for (uint8_t i = 0; i < tuning.max_amalg_count; i++)
    {
        reset_amalgame(&amalgames[i], tuning.max_pt_per_amalg, 0);
    }
}

void clean_amalgames(amalgame_finder_tuning_t tuning, amalgame_t* amalgames, uint8_t count) {
    for (uint8_t i = 0; i < count; i++)
    {
        reset_amalgame(&amalgames[i], tuning.max_pt_per_amalg, 1);
    }
}

// returns count of amalgame, and return amalgame points through amalgames_out
//TODO : handles too many pt per amalg, & too many amalgames
int calc_amalgames(amalgame_finder_tuning_t tuning, raw_lidar_t data, amalgame_t *amalgames_out) {
    uint16_t last_dist = 0, amalgs_i = 0;
    uint32_t avg_angle = 0, avg_dist = 0; //Prevent buffer overflow before averaging
    uint8_t small_angle_nb = 0, high_angle_nb = 0;
    amalgame_t* cur_amalg = &amalgames_out[0];
    //reset_amalgame(cur_amalg, tuning.max_pt_per_amalg, 1);


    for (uint16_t i = 0; i < data.count; i++)
    {
        uint16_t cur_dist = data.distances[i];
        //if outside nominal point range, outside intensity range, distance too high with previous point, too many pt in amalg
        if(cur_dist < tuning.min_dist || cur_dist > tuning.max_dist
        || (data.intensities[i] < tuning.min_intensity)
        || (last_dist != 0 && abs(last_dist - cur_dist) > tuning.max_distance_betwn_pts)
        || ((*cur_amalg).pts->count == tuning.max_pt_per_amalg)
        ) {

            if((last_dist != 0 && ((*cur_amalg).pts->count >= 
                ((last_dist > FAR_DIST_MM) ? MIN_FAR_AMALG_COUNT : MIN_CLOSE_AMALG_COUNT)))
            || ((*cur_amalg).pts->count == tuning.max_pt_per_amalg)
            ) {  //exclude too small amalgame & reset it
                (*cur_amalg).avg_angle = small_angle_nb && high_angle_nb ?
                junction_avg_angle((*cur_amalg).pts->angles, (*cur_amalg).pts->count) :
                (uint16_t) (avg_angle / (*cur_amalg).pts->count);
                (*cur_amalg).avg_dist = (uint16_t) (avg_dist / (*cur_amalg).pts->count);
                (*cur_amalg).avg_dist += OFFSET_AMALG_CENTER;
                polar_t a = {
                    .angle = (*cur_amalg).pts->angles[0],
                    .distance = (*cur_amalg).pts->distances[0]
                };
                polar_t b = {
                    .angle = (*cur_amalg).pts->angles[(*cur_amalg).pts->count-1],
                    .distance = (*cur_amalg).pts->distances[(*cur_amalg).pts->count-1]
                };
                //TODO : Accepter une tolérance à 93mm pour les balises de 80mm
                //Et expected min size en dessous d'une certaine distance (1m)
                (*cur_amalg).size = abs_dist_mm(a,b);
                cur_amalg = &amalgames_out[++amalgs_i];
            }
            if(amalgs_i == tuning.max_amalg_count - 1) { // TODO : improve management of too many amalgames
                break;
            }
            last_dist = 0;
        }
        
        if(last_dist == 0) {
            if ((*cur_amalg).pts->count < MIN_CLOSE_AMALG_COUNT)
            {
                reset_amalgame(cur_amalg, tuning.max_pt_per_amalg, 1);
            }
            avg_angle = 0, avg_dist = 0;
            small_angle_nb = 0, high_angle_nb = 0;
        }

        //If valid point, start/continue the amalgame with it
        if(cur_dist > tuning.min_dist && cur_dist < tuning.max_dist
            && (data.intensities[i] > tuning.min_intensity)) {

            (*cur_amalg).pts->angles[(*cur_amalg).pts->count] = data.angles[i];
            (*cur_amalg).pts->distances[(*cur_amalg).pts->count] = data.distances[i];
            (*cur_amalg).pts->intensities[(*cur_amalg).pts->count] = data.intensities[i];
            avg_angle += data.angles[i];
            avg_dist += data.distances[i];        
            (*cur_amalg).pts->count++;
            if(data.angles[i] < 1000) small_angle_nb = 1;
            if(data.angles[i] > 1000) high_angle_nb = 1;
    
            last_dist = data.distances[i];
        
        }
    }

    //wrap up last amalgame using if needed first amalgame
    if(last_dist != 0) {
        uint16_t first_dist = amalgames_out[0].pts->distances[0];
        if(amalgs_i > 1 && abs(last_dist - first_dist) <= tuning.max_distance_betwn_pts) {
            combine_amalg(&amalgames_out[0], cur_amalg, tuning.max_pt_per_amalg);
        }
        else {
            cur_amalg = &amalgames_out[++amalgs_i];
        }
    }

    return amalgs_i; // count of amalgames in amalgames_out
    
}

bool are_poses_equal(const pose_t* a, const pose_t* b) {
    return (a->pos.x == b->pos.x) &&
           (a->pos.y == b->pos.y) &&
           (fabsf(a->angle_rad - b->angle_rad) < ANGLE_EPSILON);
}

//polar_out need to be prealocated !
bool copy_amalgames_avg(polar_t *polar_out, int nb_amalg, amalgame_t* last_amalg) {
    // Copy avg_angle and avg_dist from amalgames_out to last_amalg
    for (int i = 0; i < nb_amalg; i++) {
        polar_out[i].angle = (last_amalg)[i].avg_angle;
        polar_out[i].distance = (last_amalg)[i].avg_dist;
    }
    return true;
}

static int8_t combine_amalg(amalgame_t* dest_amalg, amalgame_t* add_amalg, uint16_t max_count) {
    raw_lidar_t* dest = dest_amalg->pts;
    raw_lidar_t* add = add_amalg->pts;
    if(dest->count + add-> count > max_count) {
        return -2; //Error of too many points when combining amalgame
    }

    uint16_t j = dest->count;
    dest_amalg->avg_angle = 0;
    dest_amalg->avg_dist = 0;
    for (uint16_t i = 0; i < dest->count; i++)
    {
        dest_amalg->avg_angle += dest->angles[i];
        dest_amalg->avg_dist += dest->distances[i];
    }
    
    for (uint16_t i = 0; i < add->count; i++)
    {
        dest->angles[i+j] = add->angles[i];
        dest->distances[i+j] = add->distances[i];
        dest->intensities[i+j] = add->intensities[i];
        dest_amalg->avg_angle += add->angles[i];
        dest_amalg->avg_dist += add->distances[i];
    }
    dest_amalg->avg_angle /= dest->count;
    dest_amalg->avg_dist /= dest->count;
    return 1;
}

static void reset_amalgame(amalgame_t* item, uint8_t nb_pts, uint8_t need_free) {
    if(!need_free) {
        item->pts = (raw_lidar_t*) calloc(1, sizeof(raw_lidar_t));
    }
    reset_raw_lidar(item->pts, nb_pts, need_free);
    item->avg_angle = 0;
    item->avg_dist = 0;
    
}
static void reset_raw_lidar(raw_lidar_t *item, uint8_t nb_pts, uint8_t need_free) {
    if(need_free) {
        free((void *)item->angles);
        free((void *)item->distances);
        free((void *)item->intensities);
    }

    item->count = 0;
    item->angles = (uint16_t *)calloc(nb_pts, sizeof(uint16_t));    // Allocate and initialize to zero
    item->distances = (uint16_t *)calloc(nb_pts, sizeof(uint16_t)); // Allocate and initialize to zero
    item->intensities = (uint8_t *)calloc(nb_pts, sizeof(uint8_t)); 

}

static uint16_t junction_avg_angle(uint16_t* angles, uint8_t count) {
    float x = 0, y = 0;
    for (uint8_t i = 0; i < count; i++)
    {
        float angle_rad = CENTIDEGREES_TO_RADIANS((float) angles[i]);
        x+= cos(angle_rad);
        y+= sin(angle_rad);
    }
    return (uint16_t) (RADIANS_TO_CENTIDEGREES(atan2(y, x)));
    
}


