#include "motion/motion_control.impl.h"

// This file is only compiled for holonomic robots
#ifdef CONFIG_ESP_ROBOT_HOLONOMIC

#include <stddef.h>
#include <math.h>
#include <esp_log.h>

#define TAG "Motion control [DD]"
#define MOTION_STEP_RUNNING 1
#define SQRT_3_2 0.86602540378
#define PI_2 6.28318530718
#define TICKS_PER_TURN 360
#define ALLOWED_ERROR_XY 5.0
#define ALLOWED_ERROR_T 0.2
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define sign(x) ((x == 0.0) ? 1.0 : ((x) / abs(x)))

struct holonomic_data_t {
    motion_control_tuning_t *tuning;
    pose_t last_pose;
    pose_t cumulated_pose_error;
};

static void position_setpoint_to_motor(
    const pose_t *current_pose,
    const motion_control_tuning_t *tuning,
    float x_setpoint,
    float y_setpoint,
    float t_setpoint,
    float motor_setpoints[]
);

void motion_control_on_motion_target_set(motion_status_t *motion_target, const pose_t *target, const pose_t *current_pose)
{
    motion_target->pose = *target;
    motion_target->motion_step = MOTION_STEP_RUNNING;
}

void motion_control_on_init(void **motion_data, motion_control_tuning_t *tuning)
{
    struct holonomic_data_t* data = malloc(sizeof(struct holonomic_data_t));
    data->tuning = tuning;
    *motion_data = data;
}

void motion_control_on_new_target_received(void *motion_data, pose_t *current_pose)
{
    struct holonomic_data_t* data = malloc(sizeof(struct holonomic_data_t));
    ESP_LOGI(TAG, "Received new motion target.");

    data->last_pose.x = current_pose->x;
    data->last_pose.y = current_pose->y;
    data->last_pose.theta = current_pose->theta;

    data->cumulated_pose_error.x = 0.0;
    data->cumulated_pose_error.y = 0.0;
    data->cumulated_pose_error.theta = 0.0;
}

void motion_control_on_tuning_updated(void *motion_data, motion_control_tuning_t *tuning)
{
    struct holonomic_data_t* data = (struct holonomic_data_t*)motion_data;
    data->tuning = tuning;
}

void motion_control_on_pose_update(
    void *motion_data,
    pose_t *current_pose,
    const encoder_measurement_t *previous_encoder,
    const encoder_measurement_t *current_encoder
)
{
    struct holonomic_data_t* data = (struct holonomic_data_t*)motion_data;

    float channel1 = current_encoder->channel1 - previous_encoder->channel1;
    float channel2 = current_encoder->channel2 - previous_encoder->channel2;
    float channel3 = current_encoder->channel3 - previous_encoder->channel3;

    float x_ref_robot = (-channel2 + channel3) / SQRT_3_2 / 2.0;
    float y_ref_robot = (-2.0 * channel1 + channel2 + channel3) / 3.0;
    float t_ref_robot = (channel1 + channel2 + channel3) / 3.0;

    current_pose->x += x_ref_robot * cos(current_pose->theta) + y_ref_robot * sin(current_pose->theta);
    current_pose->y += - x_ref_robot * sin(current_pose->theta) + y_ref_robot * cos(current_pose->theta);
    current_pose->theta += t_ref_robot / TICKS_PER_TURN * data->tuning->wheel_radius / data->tuning->robot_radius;
}

void motion_control_on_motor_loop(void *motion_data, motion_status_t *motion_target, const pose_t *current_pose)
{
    struct holonomic_data_t* data = (struct holonomic_data_t*)motion_data;
    const pose_t target_pose = motion_target->pose;
    const pose_t last_pose = data->last_pose;

    data->cumulated_pose_error.x += (target_pose.x - current_pose->x) * MOTION_PERIOD_MS;
    data->cumulated_pose_error.y += (target_pose.y - current_pose->y) * MOTION_PERIOD_MS;
    data->cumulated_pose_error.theta += (target_pose.theta - current_pose->theta) * MOTION_PERIOD_MS;

    float motion_x = (current_pose->x - last_pose.x) / MOTION_PERIOD_MS;
    float motion_y = (current_pose->y - last_pose.y) / MOTION_PERIOD_MS;
    float motion_theta = (current_pose->theta - last_pose.theta) / MOTION_PERIOD_MS;

    if ((fabsf(target_pose.x - current_pose->x) < ALLOWED_ERROR_XY) &&
        (fabsf(target_pose.y - current_pose->y) < ALLOWED_ERROR_XY) &&
        (fabsf(target_pose.theta - current_pose->theta) < ALLOWED_ERROR_T)) {
        ESP_LOGI(TAG, "Motion finished");
        motion_target->motion_step = MOTION_STEP_DONE;
    }

    float x_setpoint = (target_pose.x - current_pose->x) * data->tuning->feedback_p
        + data->cumulated_pose_error.x * data->tuning->feedback_i
        + motion_x * data->tuning->feedback_d;
    float y_setpoint = (target_pose.y - current_pose->y) * data->tuning->feedback_p
        + data->cumulated_pose_error.y * data->tuning->feedback_i
        + motion_y * data->tuning->feedback_d;
    float t_setpoint = ((target_pose.theta - current_pose->theta) * data->tuning->feedback_p
        + data->cumulated_pose_error.theta * data->tuning->feedback_i
        + motion_theta * data->tuning->feedback_d)  / TICKS_PER_TURN * data->tuning->wheel_radius / data->tuning->robot_radius;

    data->last_pose.x = current_pose->x;
    data->last_pose.y = current_pose->y;
    data->last_pose.theta = current_pose->theta;

    float motor_setpoints[3];
    position_setpoint_to_motor(current_pose, data->tuning, x_setpoint, y_setpoint, t_setpoint, motor_setpoints);
    write_motor_speed(motor_setpoints[0], motor_setpoints[1], motor_setpoints[2]);
    ESP_LOGD(TAG, "Wrote motor speed %f %f %f", motor_setpoints[0], motor_setpoints[1], motor_setpoints[2]);
}

static float apply_friction_non_linearity(float setpoint, const motion_control_tuning_t *tuning)
{
    float clamped = abs(setpoint) * (tuning->max_speed - tuning->friction_coefficient) + tuning->friction_coefficient;
    return clamped * sign(setpoint);
}

static void position_setpoint_to_motor(
    const pose_t *current_pose,
    const motion_control_tuning_t *tuning,
    float x_setpoint,
    float y_setpoint,
    float t_setpoint,
    float motor_setpoints[]
)
{
    float x_ref_robot = x_setpoint * cos(current_pose->theta) - y_setpoint * sin(current_pose->theta);
    float y_ref_robot = x_setpoint * sin(current_pose->theta) + y_setpoint * cos(current_pose->theta);
    float t_ref_robot = t_setpoint;

    float c1 = -y_ref_robot + t_ref_robot;
    float c2 = - SQRT_3_2 * x_ref_robot + y_ref_robot / 2.0 + t_ref_robot;
    float c3 = SQRT_3_2 * x_ref_robot + y_ref_robot / 2.0 + t_ref_robot;

    float max_value = max(abs(c1), max(abs(c2), abs(c3)));
    if (max_value >= 1.0) {
        c1 /= max_value;
        c2 /= max_value;
        c3 /= max_value;
    }

    motor_setpoints[0] = apply_friction_non_linearity(c1, tuning);
    motor_setpoints[1] = apply_friction_non_linearity(c2, tuning);
    motor_setpoints[2] = apply_friction_non_linearity(c3, tuning);
}

#endif
