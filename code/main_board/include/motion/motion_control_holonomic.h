#pragma once

// X-Macro definition of motion control fields
#define MOTION_CONTROL_TUNING_FIELDS \
    X(max_speed, 0.5)


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
