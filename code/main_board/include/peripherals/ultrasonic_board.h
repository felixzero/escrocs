#pragma once

#include <stdbool.h>
#include <esp_err.h>

esp_err_t init_ultrasonic_board(void);

/**
 * Set the scanning action perimeter (angles in radian)
 */
void set_ultrasonic_scan_angle(float min_angle, float max_angle);

/**
 * Returns true if the ultrasonic mast detects an obstacle in its critical distance
 */
bool ultrasonic_has_obstacle(void);

void disable_ultrasonic_detection(void);
