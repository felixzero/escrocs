#pragma once

#include "controllers/motion_control.h"
#include "../peripherals/motor_board.h"

typedef struct {
    float wheel_radius_mm;
    float robot_diameter_mm;
    float ultra_min_speed;
    float min_speed_mps;
    float max_speed_mps;
    float acceleration_mps2;
    float emergency_acceleration_mps2;
    float ultrasonic_detection_angle;
    float ultrasonic_min_detection_distance_mm;
    float ultrasonic_ignore_distance_mm;
    float slow_approach_position_mm;
    float allowed_error_mm;
    float allowed_angle_error_rad;
    float deceleration_factor;
    float angle_feedback_p;
    float angle_feedback_i;
    float angle_feedback_d;
    float angle_max_slew_rate;
    float position_feedback_p;
    //Differential drive specific
    float left_right_balance;
} motion_control_tuning_t;

typedef struct {
    motion_control_tuning_t *tuning;
    pose_t previous_speed;
    int64_t previous_time;
    bool please_stop;
    void *current_state;
} motion_data_t;

void motion_control_on_init(motion_control_tuning_t *tuning);

void motion_control_update_pose(
    motion_data_t *motion_data,
    pose_t *current_pose,
    const encoder_measurement_t *encoder_increment
);

void motion_control_apply_speed(
    motion_data_t *motion_data,
    motion_status_t *motion_target,
    const pose_t *current_pose,
    bool force_deceleration
);

void motion_control_scanning_angles(
    motion_data_t *motion_data,
    motion_status_t *motion_target,
    const pose_t *current_pose,
    float *center_angle,
    float *cone_angle,
    bool *perform_detection
);
