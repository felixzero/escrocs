#pragma once

#include <stdbool.h>
#include <esp_err.h>

#define NUMBER_OF_US                    10
/**
 * Init ultrasound subsystem
 */
esp_err_t init_ultrasonic_board(void);



/**
 * Enable/disable channels, don't forget to disable it later !
 * @param channels : array of size NUMBER_OF_US
 */
void update_scan_channels(bool *channels);

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

/**
 * get raw values of distance from ultrasonic board of all US channels
 * @param values : array of size NUMBER_OF_US
 */
void read_all_ultrasonic_values(uint16_t *values);