#pragma once

#define MOTION_STEP_DONE 0
#define MOTION_PERIOD_MS 10
#define LIDAR_PERIOD_MS 200

#include "motion_control.h"
#include "peripherals/motor_board.h"

void motion_control_on_motion_target_set(motion_status_t *motion_target, const pose_t *target, const pose_t *current_pose);
void motion_control_on_init(void **motion_data, motion_control_tuning_t *tuning);
void motion_control_on_new_target_received(void *motion_data, pose_t *current_pose);
void motion_control_on_tuning_updated(void *motion_data, motion_control_tuning_t *tuning);
void motion_control_update_pose(
    void *motion_data,
    pose_t *current_pose,
    const encoder_measurement_t *previous_encoder,
    const encoder_measurement_t *current_encoder
);
void motion_control_on_motor_loop(void *motion_data, motion_status_t *motion_target, const pose_t *current_pose);
void motion_control_scanning_angles(
    void *motion_data,
    motion_status_t *motion_target,
    const pose_t *current_pose,
    float *min_angle,
    float *max_angle,
    bool *perform_detection
);

#define UNUSED(x) (void)(x)