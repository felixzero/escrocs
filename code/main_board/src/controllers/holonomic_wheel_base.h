#pragma once

#include "controllers/motion_control.h"
#include "../peripherals/motor_board_v3.h"

typedef struct {
    float wheel_radius_mm;
    float robot_radius_mm;
    float max_speed_mps;
    float acceleration_mps2;
    float ultrasonic_detection_angle;
    float ultrasonic_min_detection_distance_mm;
    float ultrasonic_ignore_distance_mm;
    float allowed_error_mm;
    float deceleration_factor;
} motion_control_tuning_t;

struct motion_data_t {
    motion_control_tuning_t *tuning;
    pose_t previous_speed;
    int64_t previous_time;
    bool please_stop;
};

void holonomic_wheel_base_set_values(motion_control_tuning_t *tuning);
void holonomic_wheel_base_update_pose(
    struct motion_data_t *motion_data,
    pose_t *current_pose,
    const encoder_measurement_t *encoder_increment
);
void holonomic_wheel_base_apply_speed_to_motors(
    struct motion_data_t *motion_data,
    motion_status_t *motion_target,
    const pose_t *current_pose,
    bool force_deceleration
);
void holonomic_wheel_base_get_detection_scanning_angles(
    struct motion_data_t *motion_data,
    motion_status_t *motion_target,
    const pose_t *current_pose,
    float *center_angle,
    float *cone_angle,
    bool *perform_detection
);
