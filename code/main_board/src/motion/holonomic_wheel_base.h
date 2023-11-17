#pragma once

#define MOTION_STEP_DONE 0
#define MOTION_STEP_RUNNING 1

#define MOTION_PERIOD_MS 10

#include "motion/motion_control.h"
#include "../peripherals/motor_board_v3.h"

typedef struct {
    float wheel_radius_mm;
    float robot_radius_mm;
    float max_speed_mps;
    float acceleration_mps2;
    float ultrasonic_detection_angle;
    float ultrasonic_detection_angle_offset;
    float ticks_per_turn;
    float allowed_error_ticks;
} motion_control_tuning_t;

struct motion_data_t {
    motion_control_tuning_t *tuning;
    float elapsed_time_ms;
};

void holonomic_wheel_base_set_values(motion_control_tuning_t *tuning);
void holonomic_wheel_base_update_pose(
    struct motion_data_t *motion_data,
    pose_t *current_pose,
    const encoder_measurement_t *previous_encoder,
    const encoder_measurement_t *current_encoder
);
void holonomic_wheel_base_apply_speed_to_motors(
    struct motion_data_t *motion_data,
    motion_status_t *motion_target,
    const pose_t *current_pose
);
void holonomic_wheel_base_get_detection_scanning_angles(
    struct motion_data_t *motion_data,
    motion_status_t *motion_target,
    const pose_t *current_pose,
    float *min_angle,
    float *max_angle,
    bool *perform_detection
);
