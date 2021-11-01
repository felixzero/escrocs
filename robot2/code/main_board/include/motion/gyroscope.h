#pragma once

#include <stdint.h>

/**
 * Init the gyroscope subsystem: configure the chip and load its DMP code
 */
void init_gyroscope(void);

/**
 * Read Euler angles from the gyroscope chip
 */
void read_gyroscope(float *yaw, float *pitch, float *roll);
