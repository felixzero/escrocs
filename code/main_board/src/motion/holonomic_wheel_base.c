#include "holonomic_wheel_base.h"

#include <stddef.h>
#include <math.h>
#include <esp_log.h>

#define TAG "Motion control [H]"
#define SQRT_3_2 0.86602540378
#define PI_2 6.28318530718

#define ENCODER_TO_POSITION(u, tuning) ((u) / tuning->ticks_per_turn * PI_2 * tuning->wheel_radius_mm)
#define ENCODER_TO_ANGLE(t, tuning) ((t) / tuning->ticks_per_turn * PI_2 * tuning->wheel_radius_mm / tuning->robot_radius_mm)
#define POSITION_TO_ENCODER(x, tuning) ((x) * tuning->ticks_per_turn / PI_2 / tuning->wheel_radius_mm)
#define ANGLE_TO_ENCODER(theta, tuning) ((theta) * tuning->ticks_per_turn / PI_2 / tuning->wheel_radius_mm * tuning->robot_radius_mm)

struct holonomic_data_t {
    motion_control_tuning_t *tuning;
    float elapsed_time_ms;
};

void holonomic_wheel_base_set_values(motion_control_tuning_t *tuning)
{
    tuning->wheel_radius_mm = 30.3;
    tuning->robot_radius_mm = 100.4;
    tuning->max_speed_mps = 1.0;
    tuning->acceleration_mps2 = 0.5;
    tuning->ultrasonic_detection_angle = 0.5;
    tuning->ultrasonic_detection_angle_offset = 0.0;
    tuning->ticks_per_turn = 800;
    tuning->allowed_error_ticks = 3;
}

void holonomic_wheel_base_update_pose(
    struct motion_data_t *data,
    pose_t *current_pose,
    const encoder_measurement_t *previous_encoder,
    const encoder_measurement_t *current_encoder
)
{
    float channel1 = current_encoder->channel1 - previous_encoder->channel1;
    float channel2 = current_encoder->channel2 - previous_encoder->channel2;
    float channel3 = current_encoder->channel3 - previous_encoder->channel3;

    float u_ref_robot = (-channel2 + channel3) / SQRT_3_2 / 2.0;
    float v_ref_robot = (-2.0 * channel1 + channel2 + channel3) / 3.0;
    float t_ref_robot = -(channel1 + channel2 + channel3) / 3.0;

    current_pose->x += ENCODER_TO_POSITION(u_ref_robot * cos(current_pose->theta) - v_ref_robot * sin(current_pose->theta), data->tuning);
    current_pose->y += ENCODER_TO_POSITION(u_ref_robot * sin(current_pose->theta) + v_ref_robot * cos(current_pose->theta), data->tuning);
    current_pose->theta += ENCODER_TO_ANGLE(t_ref_robot, data->tuning);
}

void holonomic_wheel_base_apply_speed_to_motors(struct motion_data_t *data, motion_status_t *motion_target, const pose_t *current_pose)
{
    const pose_t target_pose = motion_target->pose;

    float u_way_to_go = POSITION_TO_ENCODER(
        (target_pose.x - current_pose->x) * cos(current_pose->theta) + (target_pose.y - current_pose->y) * sin(current_pose->theta),
        data->tuning
    );
    float v_way_to_go = POSITION_TO_ENCODER(
        -(target_pose.x - current_pose->x) * sin(current_pose->theta) + (target_pose.y - current_pose->y) * cos(current_pose->theta),
        data->tuning
    );
    float t_way_to_go = ANGLE_TO_ENCODER(target_pose.theta - current_pose->theta, data->tuning);

    float c1 = -v_way_to_go - t_way_to_go;
    float c2 = - SQRT_3_2 * u_way_to_go + v_way_to_go / 2.0 - t_way_to_go;
    float c3 = SQRT_3_2 * u_way_to_go + v_way_to_go / 2.0 - t_way_to_go;

    if (
        (fabsf(c1) <= data->tuning->allowed_error_ticks)
        && (fabsf(c2) <= data->tuning->allowed_error_ticks)
        && (fabsf(c3) <= data->tuning->allowed_error_ticks)
    ) {
        ESP_LOGI(TAG, "Motion finished at (X=%f, Y=%f, T=%f)", current_pose->x, current_pose->y, current_pose->theta);
        motion_target->motion_step = MOTION_STEP_DONE;
        data->elapsed_time_ms = 0.0;
    }

    ESP_LOGD(TAG, "Raw waytogos: %f %f %f", c1, c2, c3);

    //float max_value = fmaxf(fabsf(c1), fmaxf(fabsf(c2), fabsf(c3)));
    float max_value = sqrtf(c1 * c1 + c2 * c2 + c3 * c3);
    data->elapsed_time_ms += MOTION_PERIOD_MS;

    float current_speed_mps = fminf(
        data->tuning->max_speed_mps,
        fminf(
            sqrtf(2 * data->tuning->acceleration_mps2 * ENCODER_TO_POSITION(max_value, data->tuning) * 1e-3),
            1e-3 * data->elapsed_time_ms * data->tuning->acceleration_mps2
        )
    );
    ESP_LOGD(TAG, "Current speed in m/s: %f", current_speed_mps);
    float current_speed_rpm = current_speed_mps * 60 / (PI_2 * data->tuning->wheel_radius_mm * 1e-3);
    c1 /= max_value / current_speed_rpm;
    c2 /= max_value / current_speed_rpm;
    c3 /= max_value / current_speed_rpm;
    ESP_LOGD(TAG, "Writing motor speed to: %f %f %f (%f)", c1, c2, c3, max_value);
    write_motor_speed_rpm(c1, c2, c3);
}

void holonomic_wheel_base_get_detection_scanning_angles(
    struct motion_data_t *data,
    motion_status_t *motion_target,
    const pose_t *current_pose,
    float *min_angle,
    float *max_angle,
    bool *perform_detection
) {
    const pose_t target_pose = motion_target->pose;
    float angle_to_target = atan2(target_pose.y - current_pose->y, target_pose.x - current_pose->x);
    
    *perform_detection = true;
    *min_angle = angle_to_target
        - current_pose->theta
        - data->tuning->ultrasonic_detection_angle
        - data->tuning->ultrasonic_detection_angle_offset;
    *max_angle = angle_to_target
        - current_pose->theta
        + data->tuning->ultrasonic_detection_angle
        - data->tuning->ultrasonic_detection_angle_offset;
}
