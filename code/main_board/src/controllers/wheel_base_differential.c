#include "controllers/wheel_base.h"
#include "controllers/motion_control.h"

// This file is only compiled for differential drive robots
#ifdef DIFFERENTIAL

#include <stddef.h>
#include <math.h>
#include <esp_log.h>

#define TAG "Motion control [DD]"
#define MOTION_STEP_INITIAL_ROTATION 1
#define MOTION_STEP_TRANSLATION 2
#define MOTION_STEP_FINAL_ROTATION 3

#define DIST_TO_RAD(x) ((x) / (0.5 * tuning->robot_diameter_mm))
#define RAD_TO_DIST(x) ((x) * (0.5 * tuning->robot_diameter_mm))

typedef struct {
    pose_t target_pose;
    const motion_control_tuning_t* tuning;
    unsigned int timer;
} rotation_state_t;

typedef struct {
    float target_x;
    float target_y;
    float target_theta;
    float sum_angle_err;
    float last_angle_err;
    const motion_control_tuning_t* tuning;
    unsigned int timer;
} translation_state_t;

static encoder_measurement_t previous_encoder_instance = {
    .channel1 = 0.0f,
    .channel2 = 0.0f,
    .channel3 = 0.0f
};

static encoder_measurement_t *previous_encoder = &previous_encoder_instance;
static pose_t previous_pose;

static void *init_rotation(float destination_angle, const pose_t *current_pose, const motion_control_tuning_t *tuning);
static bool handle_rotation(void *data, const pose_t *current_pose);
static void *init_translation(const pose_t *target_pose, const motion_control_tuning_t *tuning, const pose_t *current_pose);
static bool handle_translation(void *data, const pose_t *current_pose);
static float optimal_target_angle(const pose_t *target_pose, const pose_t *current_pose);
static bool is_xy_close(const pose_t *target_pose, const pose_t *current_pose, const motion_control_tuning_t *tuning);

void motion_control_on_init(motion_control_tuning_t *tuning)
{
    tuning->wheel_radius_mm = 50.0;
    tuning->robot_diameter_mm = 289;
    tuning->ultra_min_speed = 0.2f; //Minimum speed when correcting for rotation
    tuning->min_speed_mps = 0.25f; 
    tuning->max_speed_mps = 0.25f; //0.5
    tuning->acceleration_mps2 = 0.4f; //0.4f
    tuning->emergency_acceleration_mps2 = 0.2;
    tuning->ultrasonic_detection_angle = 1.6;
    tuning->ultrasonic_min_detection_distance_mm = 30;
    tuning->ultrasonic_ignore_distance_mm = 400;
    tuning->slow_approach_position_mm = 100;
    tuning->allowed_error_mm = 5; //3 For "is_xy_close", x2 of this value
    tuning->allowed_angle_error_rad = 0.06f; //about 3 deg
    tuning->deceleration_factor = 0.7;
    tuning->left_right_balance = 0.0; //0.05;
    tuning->angle_feedback_p = 8.0; //8.0
    tuning->angle_feedback_i = 1.0;//1.0
    tuning->angle_feedback_d = 24.0; //24.0
    tuning->angle_max_slew_rate = 0.1;
    tuning->position_feedback_p = 0.02;
}

void motion_control_update_pose(
    motion_data_t *motion_data,
    pose_t *current_pose,
    const encoder_measurement_t *current_encoder)
{
    motion_data_t* data = (motion_data_t*)motion_data;

    //ESP_LOGI(TAG, "encoder %f %f %f", current_encoder->channel1, current_encoder->channel2, current_encoder->channel3);
    float inc1 = ((current_encoder->channel2 - previous_encoder->channel2) / 360.0) * (motion_data->tuning->wheel_radius_mm * 2) * M_PI;
    float inc2 = ((current_encoder->channel3 - previous_encoder->channel3) / 360.0) * (motion_data->tuning->wheel_radius_mm * 2) * M_PI;

    previous_encoder->channel2 = current_encoder->channel2;
    previous_encoder->channel3 = current_encoder->channel3;

   //jonathan test
    //current_pose->x += (inc1 + inc2) * cosf(current_pose->theta) / 2.0;
    //current_pose->y += (inc1 + inc2) * sinf(current_pose->theta) / 2.0;
    //current_pose->theta += (inc2 - inc1) / motion_data->tuning->robot_diameter_mm;
    //OLD 
    //float inc1 = ((current_encoder->channel1 - previous_encoder->channel1) / 360.0) * (motion_data->tuning->wheel_radius_mm * 2) * M_PI;
    //float inc2 = ((current_encoder->channel2 - previous_encoder->channel2) / 360.0) * (motion_data->tuning->wheel_radius_mm * 2) * M_PI;
//
    current_pose->x += (inc1 - inc2) * cosf(current_pose->theta) / 2.0;
    current_pose->y += (inc1 - inc2) * sinf(current_pose->theta) / 2.0;
    current_pose->theta -= (inc2 + inc1) / (motion_data->tuning->robot_diameter_mm);
    //ESP_LOGI(TAG, "pose %f %f %f enc %f %f %f", current_pose->x, current_pose->y, current_pose->theta, current_encoder->channel1, current_encoder->channel2, current_encoder->channel3);
}

void motion_control_apply_speed(motion_data_t *motion_data, motion_status_t *motion_target, const pose_t *current_pose, bool force_deceleration)
{
    motion_data_t* data = (motion_data_t*)motion_data;

    //if pose has changed
    if ((isnan(previous_pose.x) && isnan(previous_pose.y) && isnan(previous_pose.theta)) ||
    previous_pose.x != motion_target->pose.x || previous_pose.y != motion_target->pose.y || previous_pose.theta != motion_target->pose.theta)
    {
        motion_target->motion_step = MOTION_STEP_INITIAL_ROTATION;
        previous_pose = motion_target->pose;
        data->current_state = NULL;
    }

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
            float target_angle = optimal_target_angle(&motion_target->pose, current_pose);
            data->current_state = init_rotation(target_angle, current_pose, data->tuning);
        }
        if (is_xy_close(&motion_target->pose, current_pose, data->tuning) 
            || handle_rotation(data->current_state, current_pose)) {
            ESP_LOGI(TAG, "Finished initial rotation; going to translate");
            motion_target->motion_step = MOTION_STEP_TRANSLATION;
            data->current_state = NULL;
        }
    } else if (motion_target->motion_step == MOTION_STEP_TRANSLATION) {
        if (data->current_state == NULL) {
            data->current_state = init_translation(&motion_target->pose, data->tuning, current_pose);
        }
        if (handle_translation(data->current_state, current_pose)) {
            ESP_LOGI(TAG, "Finished translation; going to perform final rotation");
            motion_target->motion_step = isnan(motion_target->pose.theta) ? MOTION_STEP_DONE : MOTION_STEP_FINAL_ROTATION;
            data->current_state = NULL;
        }
    }
}


void motion_control_scanning_angles(
    motion_data_t *motion_data,
    motion_status_t *motion_target,
    const pose_t *current_pose,
    float *center_angle,
    float *cone_angle,
    bool *perform_detection
) {
    motion_data_t* data = (motion_data_t*)motion_data;    

    *perform_detection = false;

    // If not performing a translation, skip obstacle check
    if ((data->current_state == NULL) || (motion_target->motion_step != MOTION_STEP_TRANSLATION)) {
        return;
    }

    translation_state_t *state = (translation_state_t*)data->current_state;

    float way_to_go = (state->target_x - current_pose->x) * cosf(current_pose->theta)
        + (state->target_y - current_pose->y) * sinf(current_pose->theta);

    *perform_detection = true;
    if (way_to_go > 0) {
        *center_angle = 0;
    } else {
        *center_angle = M_PI; // 2;
    }
    *cone_angle = data->tuning->ultrasonic_detection_angle / 2;
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
    //ESP_LOGI(TAG, "Handling rotation");
    rotation_state_t *state = (rotation_state_t*)data;
    const motion_control_tuning_t *tuning = state->tuning;

    float error_x = state->target_pose.x - current_pose->x;
    float error_y = state->target_pose.y - current_pose->y;
    ESP_LOGI(TAG, "theta %f, error_ %fx, error_y %f", current_pose->theta, error_x, error_y);
    float position_error = cosf(current_pose->theta) * error_x + sinf(current_pose->theta) * error_y;
    float position_error_correction = tuning->position_feedback_p * position_error;

    float delta_theta = remainderf(state->target_pose.theta - current_pose->theta, M_PI * 2);
    ESP_LOGI(TAG, "delta_theta %f", delta_theta);
    float absolute_speed;
    if (fabsf(delta_theta) < DIST_TO_RAD(tuning->slow_approach_position_mm)) {
        absolute_speed = tuning->min_speed_mps;
        //ESP_LOGI(TAG, "slow rotation speed %f", absolute_speed);
    } else {
        absolute_speed = fminf(tuning->acceleration_mps2 * (state->timer * MOTION_PERIOD_MS * 0.001), tuning->max_speed_mps);
        float max_speed = (tuning->min_speed_mps/(tuning->slow_approach_position_mm * 0.001)) * (RAD_TO_DIST(fabsf(delta_theta))*0.001) * tuning->deceleration_factor;
        ESP_LOGI(TAG, "max_speed: %f", max_speed);
        absolute_speed = fminf(absolute_speed, max_speed);
        absolute_speed = fmaxf(absolute_speed, tuning->min_speed_mps);
        //ESP_LOGI(TAG, "fast rotation speed %f", absolute_speed);
    }
    float rotation_speed = copysignf(absolute_speed, delta_theta);
    float wheel1 = (-rotation_speed + position_error_correction) * (1.0 - tuning->left_right_balance); //(-rotation_speed + position_error_correction) * (1.0 - tuning->left_right_balance);
    float wheel2 = (-rotation_speed - position_error_correction) * (1.0 + tuning->left_right_balance); //(-rotation_speed - position_error_correction) * (1.0 + tuning->left_right_balance);
    //ESP_LOGI(TAG, "wheel1 %f wheel2 %f", wheel1, wheel2);

    state->timer++;

    if (fabsf(delta_theta) < tuning->allowed_angle_error_rad) {
        ESP_LOGI(TAG, "Finishing rotation with error of %f", delta_theta);
        write_motor_speed_raw(0.0, 0.0, 0.0);
        return true;
    } else {
        write_motor_speed_raw(
            0.0f, wheel1, wheel2
        );
        return false;
    }
}

static void *init_translation(const pose_t *target_pose, const motion_control_tuning_t *tuning, const pose_t *current_pose)
{
    static translation_state_t state;
    state.target_x = target_pose->x;
    state.target_y = target_pose->y;
    state.target_theta = optimal_target_angle(target_pose, current_pose);
    state.last_angle_err = 0;
    state.sum_angle_err = 0;
    //state.target_theta = remainderf(target_angle - current_pose->theta, 2 * M_PI);
    ESP_LOGI(TAG, "target_angle %f", state.target_theta);
    state.tuning = tuning;
    state.timer = 0;
    return &state;
}

static bool handle_translation(void *data, const pose_t *current_pose)
{
    //ESP_LOGI(TAG, "Handling translation");
    translation_state_t *state = (translation_state_t*)data;
    const motion_control_tuning_t *tuning = state->tuning;

    float way_to_go = (state->target_x - current_pose->x) * cosf(current_pose->theta)
        + (state->target_y - current_pose->y) * sinf(current_pose->theta);
    pose_t target_pose = {
        .x = state->target_x,
        .y = state->target_y,
        .theta = 0
    };
    //float target_angle = optimal_target_angle(&target_pose, current_pose);
    //float angle_correction = tuning->angle_feedback_p * remainderf(target_angle - current_pose->theta, 2 * M_PI);
    float angle_error = remainderf(state->target_theta - current_pose->theta, 2 * M_PI);
    float derivative_angle = angle_error - state->last_angle_err;
    state->sum_angle_err += angle_error;
    state->last_angle_err = angle_error;


    //Limit max integral_angle
    float integral_angle = tuning->angle_feedback_i * state->sum_angle_err;
    if(fabsf(integral_angle) > 0.25f) {
        integral_angle = copysignf(0.25f, integral_angle);
    }
    float angle_correction = tuning->angle_feedback_p * angle_error + integral_angle + tuning->angle_feedback_d * derivative_angle;
    float target_angle = state->target_theta;

    //limit by clamping the slew rate/acceleration rate of rotation correction to avoid osciliation, except when near 0
    //if(fabsf(angle_correction) - fabsf(state->last_angle_err) > tuning->angle_max_slew_rate) {
    //    angle_correction = state->last_angle_err + copysignf(tuning->angle_max_slew_rate, angle_correction);
    //}
    if (angle_correction > 1.0 || angle_correction < -1.0) {
        angle_correction /= fabsf(angle_correction);
    }
    //if(fabsf() < 0.06f) {
    //    angle_correction = 0;
    //}

    float absolute_speed;
    if (fabsf(way_to_go) < tuning->slow_approach_position_mm) {
        absolute_speed = tuning->min_speed_mps;
    } else {
        absolute_speed = fminf(tuning->acceleration_mps2 * (state->timer * MOTION_PERIOD_MS * 0.001), tuning->max_speed_mps);
        float max_speed = (tuning->min_speed_mps/(tuning->slow_approach_position_mm * 0.001)) * (fabsf(way_to_go) * 0.001) * tuning->deceleration_factor;
        absolute_speed = fminf(absolute_speed, max_speed);
        absolute_speed = fmaxf(absolute_speed, tuning->min_speed_mps);
        //ESP_LOGI(TAG, "Speed: %f", absolute_speed);
    }
    float translation_speed = copysignf(absolute_speed, way_to_go);

    state->timer++;

    float wheel1 = (translation_speed - angle_correction * tuning->min_speed_mps) * (1.0 - tuning->left_right_balance);
    float wheel2 = (-translation_speed - angle_correction * tuning->min_speed_mps) * (1.0 + tuning->left_right_balance);
    float max_value = fmaxf(fabsf(wheel1), fabsf(wheel2));
    if (max_value > 1.0) {
        wheel1 /= max_value;
        wheel2 /= max_value;
    }


    if (wheel1 > 0 && wheel1 < tuning->ultra_min_speed) {
        wheel1 = tuning->ultra_min_speed;
    } else if (wheel1 < 0 && wheel1 > -tuning->ultra_min_speed) {
        wheel1 = -tuning->ultra_min_speed;
    }
    
    if (wheel2 > 0 && wheel2 < tuning->ultra_min_speed) {
        wheel2 = tuning->ultra_min_speed;
    } else if (wheel2 < 0 && wheel2 > -tuning->ultra_min_speed) {
        wheel2 = -tuning->ultra_min_speed;
    }

    ESP_LOGI(TAG, "wheel1 : %f, wheel2 : %f, waytogo %f, poseX %f", wheel1, wheel2, fabs(way_to_go), current_pose->x);

    if (fabs(way_to_go) < tuning->allowed_error_mm) {
        write_motor_speed_raw(0.0, 0.0, 0.0);
        return true;
    } else {
        write_motor_speed_raw(
            0.0,
            wheel1,
            wheel2
        );
        return false;
    }
}


static float optimal_target_angle(const pose_t *target_pose, const pose_t *current_pose)
{
    float target_angle = atan2f(
        target_pose->y - current_pose->y,
        target_pose->x - current_pose->x
    );

    if ((((target_pose->x - current_pose->x) * cosf(current_pose->theta))
    + ((target_pose->y - current_pose->y) * sinf(current_pose->theta))) < 0) {
        target_angle = fmodf(target_angle + M_PI, 2 * M_PI);
    }

    return target_angle;
}

static bool is_xy_close(const pose_t *target_pose, const pose_t *current_pose, const motion_control_tuning_t *tuning) {
    float dx = target_pose->x - current_pose->x;
    float dy = target_pose->y - current_pose->y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance <= tuning->allowed_error_mm*2;
}

#endif
