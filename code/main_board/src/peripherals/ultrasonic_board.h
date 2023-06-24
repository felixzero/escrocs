#pragma once

#include <stdbool.h>
#include <esp_err.h>

#define NUMBER_OF_SECTORS 10

esp_err_t init_ultrasonic_board(void);

/**
 * Set the scanning action perimeter (angles in radian)
 */
void set_ultrasonic_scan_angle(float min_angle, float max_angle);

/**
 * Set the scanning action to the whole 360° perimeter
 */
void request_full_ultrasonic_scan(void);

/**
 * Returns true if the ultrasonic mast detects an obstacle in its critical distance
 */
bool ultrasonic_has_obstacle(void);

/**
 * Get the distance in meter of an eventual obstacle in a channel
 * @returns infinity if there are no obstacles
 */
float ultrasonic_get_distance_by_sector(int channel);

/**
 * Get the angle, relative to the ultrasonic mast of a channel
 * @returns angle in radian
 */
float ultrasonic_get_angle_by_sector(int channel);

/**
 * Switch off all ultrasound mast activities
 */
void disable_ultrasonic_detection(void);
