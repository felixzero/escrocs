#pragma once

// X-Macro definition of motion control fields
#define MOTION_CONTROL_TUNING_FIELDS \
    X(max_speed, 0.5) \
    X(min_guaranteed_motion_rotation, 0.25) \
    X(min_guaranteed_motion_translation, 0.25) \
    X(slow_approach_angle, 20.0) \
    X(slow_approach_position, 200.0) \
    X(allowed_angle_error, 0.2) \
    X(allowed_position_error, 10.0) \
    X(position_feedback_p, 0.001) \
    X(angle_feedback_p, 0.06) \
    X(wheel_diameter, 100.0) \
    X(axle_width, 298.2) \
    X(acceleration, 0.4) \
    X(left_right_balance, -0.18)


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
