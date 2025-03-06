
#pragma once

#include <esp_err.h>
#include <stdbool.h>

esp_err_t init_ld06_board(void);

esp_err_t set_cone(float center_angle, float half_cone_width);
esp_err_t has_obstacle(bool *obstacle);
esp_err_t closest_obstacle(float *distance);
//esp_err_t read_all_distances(float *distances);