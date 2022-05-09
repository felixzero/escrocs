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
#define ALLOWED_ERROR_XY 5.0 // mm
#define ALLOWED_ERROR_T 0.02 // rad

#define ENCODER_TO_POSITION(u, tuning) ((u) / TICKS_PER_TURN * PI_2 * tuning->wheel_radius)
#define ENCODER_TO_ANGLE(t, tuning) ((t) / TICKS_PER_TURN * PI_2 * tuning->wheel_radius / tuning->robot_radius)
#define POSITION_TO_ENCODER(x, tuning) ((x) * TICKS_PER_TURN / PI_2 / tuning->wheel_radius)
#define ANGLE_TO_ENCODER(theta, tuning) ((theta) * TICKS_PER_TURN / PI_2 / tuning->wheel_radius * tuning->robot_radius)

struct holonomic_data_t {
    motion_control_tuning_t *tuning;
    float elapsed_time;
};

static float apply_friction_non_linearity(float setpoint, const motion_control_tuning_t *tuning);

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
    struct holonomic_data_t* data = (struct holonomic_data_t*)motion_data;
    ESP_LOGI(TAG, "Received new motion target.");
    data->elapsed_time = 0.0;
}

void motion_control_on_tuning_updated(void *motion_data, motion_control_tuning_t *tuning)
{
    struct holonomic_data_t* data = (struct holonomic_data_t*)motion_data;
    data->tuning = tuning;
}

void motion_control_update_pose(
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

    float u_ref_robot = (-channel2 + channel3) / SQRT_3_2 / 2.0;
    float v_ref_robot = (-2.0 * channel1 + channel2 + channel3) / 3.0;
    float t_ref_robot = (channel1 + channel2 + channel3) / 3.0;

    current_pose->x += ENCODER_TO_POSITION(u_ref_robot * cos(current_pose->theta) + v_ref_robot * sin(current_pose->theta), data->tuning);
    current_pose->y += ENCODER_TO_POSITION(-u_ref_robot * sin(current_pose->theta) + v_ref_robot * cos(current_pose->theta), data->tuning);
    current_pose->theta += ENCODER_TO_ANGLE(t_ref_robot, data->tuning);
}

void motion_control_on_motor_loop(void *motion_data, motion_status_t *motion_target, const pose_t *current_pose)
{
    struct holonomic_data_t* data = (struct holonomic_data_t*)motion_data;
    const pose_t target_pose = motion_target->pose;

    if ((fabsf(target_pose.x - current_pose->x) < ALLOWED_ERROR_XY) &&
        (fabsf(target_pose.y - current_pose->y) < ALLOWED_ERROR_XY) &&
        (fabsf(target_pose.theta - current_pose->theta) < ALLOWED_ERROR_T)) {
        ESP_LOGI(TAG, "Motion finished");
        motion_target->motion_step = MOTION_STEP_DONE;
    }

    float u_setpoint = POSITION_TO_ENCODER(
        (target_pose.x - current_pose->x) * cos(current_pose->theta) - (target_pose.y - current_pose->y) * sin(current_pose->theta),
        data->tuning
    );
    float v_setpoint = POSITION_TO_ENCODER(
        (target_pose.x - current_pose->x) * sin(current_pose->theta) + (target_pose.y - current_pose->y) * cos(current_pose->theta),
        data->tuning
    );
    float t_setpoint = ANGLE_TO_ENCODER(target_pose.theta - current_pose->theta, data->tuning);

    float c1 = -v_setpoint + t_setpoint;
    float c2 = - SQRT_3_2 * u_setpoint + v_setpoint / 2.0 + t_setpoint;
    float c3 = SQRT_3_2 * u_setpoint + v_setpoint / 2.0 + t_setpoint;
    ESP_LOGI(TAG, "Raw setpoints: %f %f %f", c1, c2, c3);

    float max_value = fmaxf(fabsf(c1), fmaxf(fabsf(c2), fabsf(c3)));
    data->elapsed_time += MOTION_PERIOD_MS;
    float current_speed = fminf(1.0, 0.001 * data->elapsed_time * data->tuning->acceleration);
    current_speed = fminf(current_speed, sqrtf(ENCODER_TO_POSITION(max_value, data->tuning) / data->tuning->braking_distance));
    c1 /= max_value / current_speed;
    c2 /= max_value / current_speed;
    c3 /= max_value / current_speed;

    float motor_setpoints[3];
    motor_setpoints[0] = apply_friction_non_linearity(c1, data->tuning);
    motor_setpoints[1] = apply_friction_non_linearity(c2, data->tuning);
    motor_setpoints[2] = apply_friction_non_linearity(c3, data->tuning);

    write_motor_speed(motor_setpoints[0], motor_setpoints[1], motor_setpoints[2]);
    ESP_LOGI(TAG, "Wrote motor speed %f %f %f", motor_setpoints[0], motor_setpoints[1], motor_setpoints[2]);
}

static float apply_friction_non_linearity(float setpoint, const motion_control_tuning_t *tuning)
{
    float clamped = fabsf(setpoint) * (tuning->max_speed - tuning->friction_coefficient) + tuning->friction_coefficient;
    if (setpoint > 0) {
        return clamped;
    } else if (setpoint < 0) {
        return -clamped;
    } else {
        return 0.0;
    }
    
}

#endif
