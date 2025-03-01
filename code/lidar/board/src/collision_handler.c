#include "collision_handler.h"
#include "math.h"

#define CENTIDEGTORAD(x) (x * 0.000174533f)
static float cone_width_rad = 0.1f;
static float cone_center_rad = 0.0f; 
static uint16_t obs_trig_dist_min = OBSTACLE_TRIG_DIST_MIN_MM;
static uint16_t obs_trig_dist_stop = OBSTACLE_TRIG_DIST_STOP_MM;

//int16_t read_all_amalgames(amalgame_t* amalgames) {
//    amalgames = full_amalgames;
//    return nb_amalg;
//}

bool has_obstacle() {
    for (size_t i = 0; i < nb_amalg; i++)
    {
        if (full_amalgames[i].avg_dist > obs_trig_dist_min && full_amalgames[i].avg_dist < obs_trig_dist_stop
        && fabsf(full_amalgames[i].avg_angle - cone_center_rad) < cone_width_rad) {
            return true;
        }
    }
    return false;
}

uint16_t closest_obstacle() {
    return 0;
}
esp_err_t update_cone(uint16_t angle) {
    return 0;
}
esp_err_t update_dist(uint16_t new_obstacle_trig_dist) {
    obs_trig_dist_stop = new_obstacle_trig_dist;
    return ESP_OK;
}