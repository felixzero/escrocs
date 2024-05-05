#include "holonomic_wheel_base.h"

#include <stddef.h>
#include <math.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TAG             "Motion control [H]"
#define SQRT_3_2        0.86602540378
#define TERRAIN_WIDTH   3000
#define TERRAIN_HEIGHT  2000

static bool is_projected_angle_in_terrain(const pose_t *current_pose, float angle);

void holonomic_wheel_base_set_values(motion_control_tuning_t *tuning)
{
    tuning->wheel_radius_mm = 27.0;
    tuning->robot_radius_mm = 102.0;
    tuning->max_speed_mps = 0.2;
    tuning->acceleration_mps2 = 0.1;
    tuning->ultrasonic_detection_angle = 1.0;
    tuning->ultrasonic_min_detection_distance_mm = 30;
    tuning->ultrasonic_ignore_distance_mm = 400;
    tuning->allowed_error_mm = 3;
    tuning->deceleration_factor = 0.8;
}

void holonomic_wheel_base_update_pose(
    struct motion_data_t *data,
    pose_t *current_pose,
    const encoder_measurement_t *encoder_increment
)
{
    float u_ref_robot = (-encoder_increment->channel2 + encoder_increment->channel3) / SQRT_3_2 / 2.0;
    float v_ref_robot = (-2.0 * encoder_increment->channel1 + encoder_increment->channel2 + encoder_increment->channel3) / 3.0;
    float t_ref_robot = -(encoder_increment->channel1 + encoder_increment->channel2 + encoder_increment->channel3) / 3.0;

    current_pose->x += (u_ref_robot * cos(current_pose->theta) - v_ref_robot * sin(current_pose->theta)) * 2 * M_PI * data->tuning->wheel_radius_mm;
    current_pose->y += (u_ref_robot * sin(current_pose->theta) + v_ref_robot * cos(current_pose->theta)) * 2 * M_PI * data->tuning->wheel_radius_mm;
    current_pose->theta += 2 * M_PI * t_ref_robot * data->tuning->wheel_radius_mm / data->tuning->robot_radius_mm;
}

void holonomic_wheel_base_apply_speed_to_motors(
    struct motion_data_t *data,
    motion_status_t *motion_target,
    const pose_t *current_pose,
    bool force_deceleration
) {
    int64_t current_time = esp_timer_get_time();

    const pose_t target_pose = motion_target->pose;
    pose_t target_displacement = {
        .x = target_pose.x - current_pose->x,
        .y = target_pose.y - current_pose->y,
        .theta = data->tuning->robot_radius_mm * (target_pose.theta - current_pose->theta)
    };
    float target_displacement_norm = sqrtf(
        target_displacement.x * target_displacement.x
        + target_displacement.y * target_displacement.y
        + target_displacement.theta * target_displacement.theta
    );
    float max_speed_norm = fminf(
        data->tuning->max_speed_mps,
        data->tuning->deceleration_factor * sqrtf(2 * data->tuning->acceleration_mps2 * target_displacement_norm * 1e-3)
    );
    if (force_deceleration || data->please_stop) {
        max_speed_norm = 0;
    }
    if (target_displacement_norm < data->tuning->allowed_error_mm) {
        ESP_LOGI(TAG, "Motion finished at (X=%f, Y=%f, T=%f)", current_pose->x, current_pose->y, current_pose->theta);
        motion_target->motion_step = MOTION_STEP_DONE;
        write_motor_speed_rad_s(0, 0, 0);
        data->previous_speed.x = 0;
        data->previous_speed.y = 0;
        data->previous_speed.theta = 0;
        return;
    }

    pose_t target_speed = {
        .x = max_speed_norm * target_displacement.x / target_displacement_norm,
        .y = max_speed_norm * target_displacement.y / target_displacement_norm,
        .theta = max_speed_norm * target_displacement.theta / target_displacement_norm,
    };
    pose_t speed_difference = {
        .x = target_speed.x - data->previous_speed.x,
        .y = target_speed.y - data->previous_speed.y,
        .theta = target_speed.theta - data->previous_speed.theta,
    };
    float speed_difference_norm = sqrtf(
        speed_difference.x * speed_difference.x
        + speed_difference.y * speed_difference.y
        + speed_difference.theta * speed_difference.theta
    );
    float time_difference_s = (current_time - data->previous_time) * 1e-6;
    data->previous_time = current_time;
    if (speed_difference_norm > data->tuning->acceleration_mps2 * time_difference_s) {
        speed_difference.x = speed_difference.x / speed_difference_norm * data->tuning->acceleration_mps2 * time_difference_s;
        speed_difference.y = speed_difference.y / speed_difference_norm * data->tuning->acceleration_mps2 * time_difference_s;
        speed_difference.theta = speed_difference.theta / speed_difference_norm * data->tuning->acceleration_mps2 * time_difference_s;
    }
    pose_t new_speed = {
        .x = data->previous_speed.x + speed_difference.x,
        .y = data->previous_speed.y + speed_difference.y,
        .theta = data->previous_speed.theta + speed_difference.theta,
    };
    data->previous_speed = new_speed;

    float u = new_speed.x * cosf(current_pose->theta) + new_speed.y * sinf(current_pose->theta);
    float v = -new_speed.x * sinf(current_pose->theta) + new_speed.y * cosf(current_pose->theta);
    float t = new_speed.theta;
    float c1 = -v - t;
    float c2 = - SQRT_3_2 * u + v / 2.0 - t;
    float c3 = SQRT_3_2 * u + v / 2.0 - t;
    write_motor_speed_rad_s(
        c1 / (data->tuning->wheel_radius_mm * 1e-3),
        c2 / (data->tuning->wheel_radius_mm * 1e-3),
        c3 / (data->tuning->wheel_radius_mm * 1e-3)
    );
}

void holonomic_wheel_base_get_detection_scanning_angles(
    struct motion_data_t *data,
    motion_status_t *motion_target,
    const pose_t *current_pose,
    float *center_angle,
    float *cone_angle,
    bool *perform_detection
) {
    const pose_t target_pose = motion_target->pose;
    float distance_to_target_sq = powf(target_pose.y - current_pose->y, 2) + powf(target_pose.x - current_pose->x, 2);
    float angle_to_target = atan2(target_pose.y - current_pose->y, target_pose.x - current_pose->x);
    
    *perform_detection = distance_to_target_sq > powf(data->tuning->ultrasonic_min_detection_distance_mm, 2);
    *center_angle = angle_to_target - current_pose->theta;
    *cone_angle = 2 * data->tuning->ultrasonic_detection_angle;

    // Eliminate the portion of cone outside the terrain
    float left_angle = *center_angle - *cone_angle / 2;
    float right_angle = *center_angle + *cone_angle / 2;
    const float ANGLE_INCREMENT = 0.05;
    while (!is_projected_angle_in_terrain(current_pose, left_angle) && (left_angle < right_angle)) {
        left_angle += ANGLE_INCREMENT;
    }
    while (!is_projected_angle_in_terrain(current_pose, right_angle) && (left_angle < right_angle)) {
        right_angle -= ANGLE_INCREMENT;
    }
    *center_angle = (left_angle + right_angle) / 2;
    *cone_angle = right_angle - left_angle;
}

static bool is_projected_angle_in_terrain(const pose_t *current_pose, float angle)
{
    float projected_x = current_pose->x + cosf(angle) + current_pose->theta;
    float projected_y = current_pose->y + sinf(angle) + current_pose->theta;

    return (projected_x > 0) && (projected_x < TERRAIN_WIDTH) && (projected_y > 0) && (projected_y < TERRAIN_HEIGHT);
}
