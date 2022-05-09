#include "motion/motion_control.impl.h"

// This file is only compiled for differential drive robots
#ifdef CONFIG_ESP_ROBOT_DIFFERENTIAL_DRIVE

#include <stddef.h>
#include <math.h>
#include <esp_log.h>

#define TAG "Motion control [DD]"
#define MOTION_STEP_INITIAL_ROTATION 1
#define MOTION_STEP_TRANSLATION 2
#define MOTION_STEP_FINAL_ROTATION 3

struct differential_drive_data_t {
    motion_control_tuning_t *tuning;
    void *current_state;
};

typedef struct {
    pose_t target_pose;
    const motion_control_tuning_t* tuning;
    unsigned int timer;
} rotation_state_t;

typedef struct {
    float target_x;
    float target_y;
    const motion_control_tuning_t* tuning;
    unsigned int timer;
} translation_state_t;

static void *init_rotation(float destination_angle, const pose_t *current_pose, const motion_control_tuning_t *tuning);
static bool handle_rotation(void *data, const pose_t *current_pose);
static void *init_translation(const pose_t *target_pose, const motion_control_tuning_t *tuning);
static bool handle_translation(void *data, const pose_t *current_pose);

void motion_control_on_motion_target_set(motion_status_t *motion_target, const pose_t *target, const pose_t *current_pose)
{
    motion_target->pose = *target;

    if (isnan(target->x) && isnan(target->y) && isnan(target->theta)) { // No change is asked
        motion_target->motion_step = MOTION_STEP_DONE;
    } else if (isnan(target->x) && isnan(target->y)) { // Pure rotation
        motion_target->motion_step = MOTION_STEP_FINAL_ROTATION;
    } else { // Regular motion
        motion_target->motion_step = MOTION_STEP_INITIAL_ROTATION;
    }

    // Fill X, Y NaN values
    if (isnan(motion_target->pose.x)) {
        motion_target->pose.x = current_pose->x;
    }
    if (isnan(motion_target->pose.y)) {
        motion_target->pose.y = current_pose->y;
    }
}

void motion_control_on_init(void **motion_data, motion_control_tuning_t *tuning)
{
    struct differential_drive_data_t* data = malloc(sizeof(struct differential_drive_data_t));
    data->tuning = tuning;
    data->current_state = NULL;
    *motion_data = data;
}

void motion_control_on_new_target_received(void *motion_data, pose_t *current_pose)
{
    struct differential_drive_data_t* data = (struct differential_drive_data_t*)motion_data;
    data->current_state = NULL;
}

void motion_control_on_tuning_updated(void *motion_data, motion_control_tuning_t *tuning)
{
    struct differential_drive_data_t* data = (struct differential_drive_data_t*)motion_data;
    data->tuning = tuning;
}

void motion_control_update_pose(
    void *motion_data,
    pose_t *current_pose,
    const encoder_measurement_t *previous_encoder,
    const encoder_measurement_t *current_encoder
)
{
    struct differential_drive_data_t* data = (struct differential_drive_data_t*)motion_data;

    float inc1 = (current_encoder->channel1 - previous_encoder->channel1) * data->tuning->wheel_diameter / 360.0 * M_PI;
    float inc2 = (current_encoder->channel2 - previous_encoder->channel2) * data->tuning->wheel_diameter / 360.0 * M_PI;

    current_pose->x += (inc2 - inc1) * cos(current_pose->theta * M_PI / 180.0) / 2.0;
    current_pose->y += (inc2 - inc1) * sin(current_pose->theta * M_PI / 180.0) / 2.0;
    current_pose->theta -= (inc2 + inc1) / data->tuning->axle_width * 180.0 / M_PI;
}

void motion_control_on_motor_loop(void *motion_data, motion_status_t *motion_target, const pose_t *current_pose)
{
    struct differential_drive_data_t* data = (struct differential_drive_data_t*)motion_data;

    if (motion_target->motion_step == MOTION_STEP_FINAL_ROTATION) {
        if (data->current_state == NULL) {
            data->current_state = init_rotation(motion_target->pose.theta, current_pose, data->tuning);
        }
        if (handle_rotation(data->current_state, current_pose)) {
            ESP_LOGI(TAG, "Finished final rotation; idling");
            motion_target->motion_step = MOTION_STEP_DONE;
            data->current_state = NULL;
        }
    } else if (motion_target->motion_step == MOTION_STEP_INITIAL_ROTATION) {
        if (data->current_state == NULL) {
            float target_angle = 180.0 / M_PI * atan2f(
                motion_target->pose.y - current_pose->y,
                motion_target->pose.x - current_pose->x
            );
            data->current_state = init_rotation(target_angle, current_pose, data->tuning);
        }
        if (handle_rotation(data->current_state, current_pose)) {
            ESP_LOGI(TAG, "Finished initial rotation; going to translate");
            motion_target->motion_step = MOTION_STEP_TRANSLATION;
            data->current_state = NULL;
        }
    } else if (motion_target->motion_step == MOTION_STEP_TRANSLATION) {
        if (data->current_state == NULL) {
            data->current_state = init_translation(&motion_target->pose, data->tuning);
        }
        if (handle_translation(data->current_state, current_pose)) {
            ESP_LOGI(TAG, "Finished translation; going to perform final rotation");
            motion_target->motion_step = isnan(motion_target->pose.theta) ? MOTION_STEP_DONE : MOTION_STEP_FINAL_ROTATION;
            data->current_state = NULL;
        }
    }
}

static void *init_rotation(float destination_angle, const pose_t *current_pose, const motion_control_tuning_t *tuning)
{
    static rotation_state_t state;
    state.target_pose.x = current_pose->x;
    state.target_pose.y = current_pose->y;
    state.target_pose.theta = destination_angle;
    state.tuning = tuning;
    state.timer = 0;
    return &state;
}

static bool handle_rotation(void *data, const pose_t *current_pose)
{
    rotation_state_t *state = (rotation_state_t*)data;
    const motion_control_tuning_t *tuning = state->tuning;

    float error_x = state->target_pose.x - current_pose->x;
    float error_y = state->target_pose.y - current_pose->y;
    float position_error = cosf(current_pose->theta) * error_x + sinf(current_pose->theta) * error_y;
    float position_error_correction = tuning->position_feedback_p * position_error;

    float delta_theta = state->target_pose.theta - current_pose->theta;
    float absolute_speed;
    if (fabsf(delta_theta) < tuning->slow_approach_angle) {
        absolute_speed = tuning->min_guaranteed_motion_rotation;
    } else {
        absolute_speed = fminf(tuning->acceleration * (state->timer * MOTION_PERIOD_MS * 0.001), tuning->max_speed);
    }
    float rotation_speed = copysignf(absolute_speed, delta_theta);

    state->timer++;

    if (fabsf(delta_theta) < tuning->allowed_angle_error) {
        write_motor_speed(0.0, 0.0, 0.0);
        return true;
    } else {
        write_motor_speed(
            (-rotation_speed - position_error_correction) * (1.0 + tuning->left_right_balance),
            (-rotation_speed + position_error_correction) * (1.0 - tuning->left_right_balance),
            0.0
        );
        return false;
    }
}

static void *init_translation(const pose_t *target_pose, const motion_control_tuning_t *tuning)
{
    static translation_state_t state;
    state.target_x = target_pose->x;
    state.target_y = target_pose->y;
    state.tuning = tuning;
    state.timer = 0;
    return &state;
}

static bool handle_translation(void *data, const pose_t *current_pose)
{
    translation_state_t *state = (translation_state_t*)data;
    const motion_control_tuning_t *tuning = state->tuning;

    float way_to_go = (state->target_x - current_pose->x) * cosf(current_pose->theta * M_PI / 180.0)
        + (state->target_y - current_pose->y) * sinf(current_pose->theta * M_PI / 180.0);
    float target_angle = 180.0 / M_PI * atan2f(state->target_y - current_pose->y, state->target_x - current_pose->x);
    float angle_correction = tuning->angle_feedback_p * remainderf(target_angle - current_pose->theta, 360.0);

    float absolute_speed;
    if (fabsf(way_to_go) < tuning->slow_approach_position) {
        absolute_speed = tuning->min_guaranteed_motion_translation;
    } else {
        absolute_speed = fminf(tuning->acceleration * (state->timer * MOTION_PERIOD_MS * 0.001), tuning->max_speed);
    }
    float translation_speed = copysignf(absolute_speed, way_to_go);

    state->timer++;

    if (fabs(way_to_go) < tuning->allowed_position_error) {
        write_motor_speed(0.0, 0.0, 0.0);
        return true;
    } else {
        write_motor_speed(
            (-translation_speed - angle_correction) * (1.0 + tuning->left_right_balance),
            (translation_speed - angle_correction) * (1.0 - tuning->left_right_balance),
            0.0
        );
        return false;
    }
}

#endif
