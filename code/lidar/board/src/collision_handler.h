#pragma once

#include "../loca_lidar/amalgame.h"
#include "lidar.h"

#include "stdbool.h"
#include "esp_err.h"

#define OBSTACLE_TRIG_DIST_MIN_MM 100
#define OBSTACLE_TRIG_DIST_STOP_MM 500

/*
Returns : number of amalgames
*/
//int16_t read_all_amalgames(amalgame_t* amalgames);

bool has_obstacle();
//returns in mm the closest amalgame distance to the lidar
uint16_t closest_obstacle();
/*
angle in ...
*/
esp_err_t update_cone(uint16_t angle);
/*
dist in mm
*/
esp_err_t update_dist(uint16_t new_obstacle_trig_dist);