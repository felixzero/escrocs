#pragma once

// X-Macro definition of motion control fields
#define MOTION_CONTROL_TUNING_FIELDS \
    X(wheel_radius, 28.5) \
    X(robot_radius, 122.26) \
    X(max_speed, 1.0) \
    X(friction_coefficient, 0.25) \
    X(acceleration, 1.0) \
    X(braking_distance, 300.0)

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
