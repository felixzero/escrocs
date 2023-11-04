#pragma once

// X-Macro definition of motion control fields
#define MOTION_CONTROL_TUNING_FIELDS \
    X(obstacle_no_detection_distance, 5.0) \
    X(ultrasonic_detection_angle, 0.5) \
    X(ultrasonic_detection_angle_offset, 0.0) \
    X(wheel_radius, 32.5) \
    X(robot_radius, 139.0) \
    X(max_speed, 0.7) \
    X(friction_coefficient, 0.0) \
    X(acceleration, 0.5) \
    X(braking_distance, 100.0) \
    X(minimum_guaranteed_motion, 0.05)

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
