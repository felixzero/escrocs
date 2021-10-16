#pragma once

#include <stdint.h>
#include <stddef.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * Init the gyroscope subsystem: configure the chip and load its DMP code
 */
void init_gyroscope(void);

/**
 * Read Euler angles from the gyroscope chip
 */
void read_gyroscope(float *yaw, float *pitch, float *roll);

/**
 * Get yaw with a drift correction
 */
float get_gyroscope_corrected_angle(size_t channel);

/**
 * Capture positions and calculate drift; the robot should not be moving
 */
void calibrate_gyroscope_drift(TickType_t calibration_time);

/**
 * Define current angle as zero
 */
void zero_out_gyroscope(void);