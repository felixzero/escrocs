#pragma once

#include "../loca_lidar/amalgame.h"
#include "../loca_lidar/loca_lidar.h"
#include "lidar.h"

#include "stdbool.h"
#include "esp_err.h"

#define OBSTACLE_TRIG_DIST_MIN_MM 10 //100
#define OBSTACLE_TRIG_DIST_STOP_MM 500

extern polar_t polar_array[MAX_AMALG_COUNT];
extern uint16_t nb_cur_amalg;

//To be called in the main
void update_amalgames_task(void *pvParameters);


bool has_obstacle();
//returns in mm the closest amalgame distance to the lidar
//The object may not be an "obstacle" (which means < dist_stop)
uint16_t closest_obstacle_dist();
/*
angle in ...
*/
esp_err_t update_cone(float center_cone, float width_cone);
/*
dist in mm
*/
esp_err_t update_dist(uint16_t new_obstacle_trig_dist);