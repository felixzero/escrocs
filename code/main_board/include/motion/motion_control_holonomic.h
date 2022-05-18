#pragma once

// X-Macro definition of motion control fields
#define MOTION_CONTROL_TUNING_FIELDS \
    X(lidar_filter, 0.1) \
    X(obstacle_distance, 60.0) \
    X(wheel_radius, 32.5) \
    X(robot_radius, 135.0) \
    X(max_speed, 0.6) \
    X(friction_coefficient, 0.10) \
    X(acceleration, 1.0) \
    X(braking_distance, 50.0) \
    X(minimum_guaranteed_motion, 0.3)

/**
 * Tuning parameters of the motor feedback control
 * Motor voltages are unitless (in range [0, 1])
 * Distances are in mm
 * Times are in s
 * All other units derive from those
 */
#define X(name, default_value) float name;
typedef struct {
    MOTION_CONTROL_TUNING_FIELDS
} motion_control_tuning_t;
#undef X
