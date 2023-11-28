#pragma once

#include <stdbool.h>
#include <esp_err.h>

/**
 * Init ultrasound subsystem
 */
esp_err_t init_ultrasonic_board(void);

/**
 * Set the scanning action perimeter (angles in radian)
 * @return The number of active channels
 */
int set_ultrasonic_scan_angle(float center_angle, float cone);

/**
 * Configure the display distance on the LED display (<critical is red; >safe is green)
 */
void set_ultrasonic_display_distances(float critical_distance, float safe_distance);

/**
 * Set the scan repetition period between two channel scans
 */
void set_ultrasonic_repetition_period(int period_ms);

/**
 * Set the distance beyond which the returned value is timed out
 */
void set_ultrasonic_timeout_distance(float distance_mm);

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
 * Disable or enable all ultrasound channels
 * @return The number of active channels
 */
int disable_all_ultrasonic_detection(void);
int enable_all_ultrasonic_detection(void);
