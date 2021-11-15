#pragma once

#include <stdbool.h>

/**
 * Tuning parameters of the motor feedback control
 * Motor voltages are unitless (in range [0, 1])
 * Distances are in mm
 * Times are in s
 * All other units derive from those
 * @param pid_p (and i and d) PID coefficients
 * @param friction_threshold Minimum voltage to move robot
 * @param max_speed Maximum motor voltage
 * @param acceleration_rate Change of voltage during acceleration phase
 */
typedef struct {
    float pid_p;
    float pid_i;
    float pid_d;
    float friction_threshold;
    float max_speed;
    float acceleration_rate;
} motion_control_tuning_t;

/**
 * Init motion control subsystem
 */
void init_motion_control(void);

/**
 * Set a (x, y, theta) target in (mm, mm, deg)
 */
void set_motion_target(float target_x, float target_y, float target_theta);

/**
 * Get current (x, y, theta) position
 */
void get_current_position(float *position_x, float *position_y, float *position_theta);

/**
 * Stop all motion
 */
void stop_motion(void);

/**
 * Check if the robot is still in motion
 */
bool is_motion_done(void);

/**
 * Apply feedback control tuning parameters
 */
void set_motion_control_tuning(motion_control_tuning_t tuning);
