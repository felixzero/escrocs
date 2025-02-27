#pragma once

#include "../loca_lidar/amalgame.h"
#include "stdbool.h"
#include "esp_err.h"

extern amalgame_t* current_amalgames;
extern uint16_t max_amalg_length;
static uint16_t obstacle_trig_dist = 0;
esp_err_t init_collision(amalgame_t* pointer_to_amalgames, uint16_t maximum_amalg_length);
/*
Returns : number of amalgames
*/
int16_t read_all_amalgames(amalgame_t* amalgames);

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