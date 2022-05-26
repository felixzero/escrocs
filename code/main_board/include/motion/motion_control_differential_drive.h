#pragma once

// X-Macro definition of motion control fields
#define MOTION_CONTROL_TUNING_FIELDS \
    X(lidar_filter, 0.0) \
    X(obstacle_distance_front, 50.0) \
    X(obstacle_distance_back, 45.0) \
    X(max_speed, 0.4) \
    X(min_guaranteed_motion_rotation, 0.3) \
    X(min_guaranteed_motion_translation, 0.3) \
    X(slow_approach_angle, 0.25) \
    X(slow_approach_position, 200.0) \
    X(allowed_angle_error, 0.01) \
    X(allowed_position_error, 10.0) \
    X(position_feedback_p, 0.00) \
    X(angle_feedback_p, 40.0) \
    X(wheel_diameter, 98.6) \
    X(axle_width, 286.0) \
    X(acceleration, 0.3) \
    X(left_right_balance, -0.06)


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
