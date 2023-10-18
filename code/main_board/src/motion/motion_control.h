#pragma once

#include <stdbool.h>
#include <sdkconfig.h>

/**
 * Pose of the robot
 * @param x (and y) position in mm
 * @param theta angle in degree
 * NaN values signify no change (for x and y) and don't care (for theta)
 */
typedef struct {
    float x;
    float y;
    float theta;
} pose_t;

typedef struct {
    pose_t pose;
    bool perform_detection;
    int motion_step;
} motion_status_t;

#if defined(CONFIG_ESP_ROBOT_HOLONOMIC)
#include "motion/motion_control_holonomic.h"
#endif

/**
 * Init motion control subsystem
 */
void init_motion_control(bool reversed_side);

/**
 * Set a (x, y, theta) target in (mm, mm, deg)
 */
void set_motion_target(const pose_t *target, bool perform_detection);

/**
 * Get current (x, y, theta) pose in (mm, mm, deg)
 */
pose_t get_current_pose(void);

/**
 * Declare current pose as equal to a known value
 * Used to (re-)initialize the motion control system
 */
void overwrite_current_pose(const pose_t *target);

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
void set_motion_control_tuning(const motion_control_tuning_t *tuning);
