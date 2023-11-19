#pragma once

#include <stdbool.h>
#include <esp_err.h>

/**
 * Init ultrasound subsystem
 */
esp_err_t init_ultrasonic_board(void);

/**
 * Set the scanning action perimeter (angles in radian)
 */
void set_ultrasonic_scan_angle(float min_angle, float max_angle);

/**
 * Send the trigger to perform the scan
 * The ultrasonic board will not be responsive for a while
 */
void ultrasonic_perform_scan(void);

/**
 * Returns true if the ultrasonic mast detects an obstacle in its critical distance
 */
bool ultrasonic_has_obstacle(void);

/**
 * Disable all ultrasound channels
 */
void disable_ultrasonic_detection(void);
