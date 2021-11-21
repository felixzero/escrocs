#include "motion/motion_control.h"
#include "motion/motor_board.h"
#include "system/task_priority.h"
#include "utils/pid.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <math.h>

#define MOTION_PERIOD_MS 10

#define TAG "Motion control"

#define MOTION_STEP_INITIAL_ROTATION 0
#define MOTION_STEP_TRANSLATION 1
#define MOTION_STEP_FINAL_ROTATION 2
#define MOTION_STEP_DONE 3

typedef struct {
    pose_t pose;
    int motion_step;
} motion_target_t;

typedef struct {
    pose_t current_pose;
    bool motion_done;
} motion_information_t;

typedef struct {
    pose_t target_pose;
    const motion_control_tuning_t* tuning;
} rotation_state_t;

typedef struct {
    float target_x;
    float target_y;
    const motion_control_tuning_t* tuning;
} translation_state_t;

static QueueHandle_t set_target_queue, motion_information_queue, tuning_queue;

static void motion_control_task(void *parameters);
static void *init_rotation(float destination_angle, const pose_t *current_pose, const motion_control_tuning_t *tuning);
static bool handle_rotation(void *data, const pose_t *current_pose);
static void *init_translation(const pose_t *target_pose, const motion_control_tuning_t *tuning);
static bool handle_translation(void *data, const pose_t *current_pose);
static void update_pose(
    pose_t *current_pose,
    const encoder_measurement_t *previous,
    const encoder_measurement_t *current,
    const motion_control_tuning_t* tuning
);

void init_motion_control(void)
{
    set_target_queue = xQueueCreate(1, sizeof(motion_target_t));
    motion_information_queue = xQueueCreate(1, sizeof(motion_information_t));
    tuning_queue = xQueueCreate(1, sizeof(motion_control_tuning_t));

    TaskHandle_t task;
    xTaskCreate(motion_control_task, "motor_board", TASK_STACK_SIZE, NULL, MOTION_CONTROL_PRIORITY, &task);
}

void set_motion_target(const pose_t *target)
{
    motion_target_t motion_target;
    motion_target.pose = *target;

    if (isnan(target->x) && isnan(target->y) && isnan(target->theta)) { // No change is asked
        motion_target.motion_step = MOTION_STEP_DONE;
    } else if (isnan(target->x) && isnan(target->y)) { // Pure rotation
        motion_target.motion_step = MOTION_STEP_FINAL_ROTATION;
    } else { // Regular motion
        motion_target.motion_step = MOTION_STEP_INITIAL_ROTATION;
    }

    // Fill X, Y NaN values
    pose_t current_pose = get_current_pose();
    if (isnan(motion_target.pose.x)) {
        motion_target.pose.x = current_pose.x;
    }
    if (isnan(motion_target.pose.y)) {
        motion_target.pose.y = current_pose.y;
    }

    // Send request to task
    ESP_LOGI(TAG, "Setting target to: %f %f %f", target->x, target->y, target->theta);
    xQueueOverwrite(set_target_queue, &motion_target);
}

pose_t get_current_pose(void)
{
    motion_information_t information;
    xQueuePeek(motion_information_queue, &information, 0);
    return information.current_pose;
}

void stop_motion(void)
{
    pose_t stop_pose = {
        .x = NAN,
        .y = NAN,
        .theta = NAN
    };
    set_motion_target(&stop_pose);
}

bool is_motion_done(void)
{
    motion_information_t information;
    xQueuePeek(motion_information_queue, &information, 0);
    
    return information.motion_done;
}

void set_motion_control_tuning(const motion_control_tuning_t *tuning)
{
    xQueueOverwrite(tuning_queue, tuning);
}

static void motion_control_task(void *parameters)
{
    TickType_t iteration_time = xTaskGetTickCount();
    pose_t current_pose;
    motion_target_t motion_target = {
        .motion_step = MOTION_STEP_DONE
    };
    motion_control_tuning_t tuning;
    encoder_measurement_t previous_encoder;
    void *motion_data = NULL;

    // Set default tuning parameters
    #define X(name, value) tuning.name = value;
    MOTION_CONTROL_TUNING_FIELDS
    #undef X

    read_encoders(&previous_encoder);

    while (true) {
        vTaskDelayUntil(&iteration_time, MOTION_PERIOD_MS / portTICK_PERIOD_MS);

        // Retrieve latest parameters
        if (xQueueReceive(set_target_queue, &motion_target, 0)) {
            motion_data = NULL;
        }
        xQueueReceive(tuning_queue, &tuning, 0);

        if (motion_target.motion_step == MOTION_STEP_DONE) {
            write_motor_speed(0.0, 0.0, 0.0);
        } else if (motion_target.motion_step == MOTION_STEP_FINAL_ROTATION) {
            if (motion_data == NULL) {
                motion_data = init_rotation(motion_target.pose.theta, &current_pose, &tuning);
            }
            if (handle_rotation(motion_data, &current_pose)) {
                ESP_LOGI(TAG, "Finished final rotation; idling");
                motion_target.motion_step = MOTION_STEP_DONE;
                motion_data = NULL;
            }
        } else if (motion_target.motion_step == MOTION_STEP_INITIAL_ROTATION) {
            if (motion_data == NULL) {
                float target_angle = 180.0 / M_PI * atan2f(
                    motion_target.pose.y - current_pose.y,
                    motion_target.pose.x - current_pose.x
                );
                motion_data = init_rotation(target_angle, &current_pose, &tuning);
            }
            if (handle_rotation(motion_data, &current_pose)) {
                ESP_LOGI(TAG, "Finished initial rotation; going to translate");
                motion_target.motion_step = MOTION_STEP_TRANSLATION;
                motion_data = NULL;
            }
        } else if (motion_target.motion_step == MOTION_STEP_TRANSLATION) {
            if (motion_data == NULL) {
                motion_data = init_translation(&motion_target.pose, &tuning);
            }
            if (handle_translation(motion_data, &current_pose)) {
                ESP_LOGI(TAG, "Finished translation; going to perform final rotation");
                motion_target.motion_step = isnan(motion_target.pose.theta) ? MOTION_STEP_DONE : MOTION_STEP_FINAL_ROTATION;
                motion_data = NULL;
            }
        }

        // Update pose according to encoders
        encoder_measurement_t encoder;
        read_encoders(&encoder);
        update_pose(&current_pose, &previous_encoder, &encoder, &tuning);
        previous_encoder = encoder;

        // Broadcast current pose
        motion_information_t information;
        information.current_pose = current_pose;
        information.motion_done = false;
        xQueueOverwrite(motion_information_queue, &information);
    }
}

static void *init_rotation(float destination_angle, const pose_t *current_pose, const motion_control_tuning_t *tuning)
{
    static rotation_state_t state;
    state.target_pose.x = current_pose->x;
    state.target_pose.y = current_pose->y;
    state.target_pose.theta = destination_angle;
    state.tuning = tuning;
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
    float rotation_speed = copysignf(
        (fabsf(delta_theta) < tuning->slow_approach_angle) ? tuning->min_guaranteed_motion_rotation : tuning->max_speed,
        delta_theta
    );

    if (fabsf(delta_theta) < tuning->allowed_angle_error) {
        write_motor_speed(0.0, 0.0, 0.0);
        return true;
    } else {
        write_motor_speed(-rotation_speed - position_error_correction, -rotation_speed + position_error_correction, 0.0);
        return false;
    }
}

static void *init_translation(const pose_t *target_pose, const motion_control_tuning_t *tuning)
{
    static translation_state_t state;
    state.target_x = target_pose->x;
    state.target_y = target_pose->y;
    state.tuning = tuning;
    return &state;
}

static bool handle_translation(void *data, const pose_t *current_pose)
{
    translation_state_t *state = (translation_state_t*)data;
    const motion_control_tuning_t *tuning = state->tuning;

    float way_to_go = (state->target_x - current_pose->x) * cosf(current_pose->theta * M_PI / 180.0)
        + (state->target_y - current_pose->y) * sinf(current_pose->theta * M_PI / 180.0);
    float target_angle = atan2f(state->target_y - current_pose->y, state->target_x - current_pose->x);
    float angle_correction = tuning->angle_feedback_p * (target_angle - current_pose->theta);

    float translation_speed = copysignf(
        (fabsf(way_to_go) < tuning->slow_approach_position) ? tuning->min_guaranteed_motion_translation : tuning->max_speed,
        way_to_go
    );

    if (fabs(way_to_go) < tuning->allowed_position_error) {
        write_motor_speed(0.0, 0.0, 0.0);
        return true;
    } else {
        write_motor_speed(-translation_speed - angle_correction, translation_speed - angle_correction, 0.0);
        return false;
    }
}

static void update_pose(
    pose_t *current_pose,
    const encoder_measurement_t *previous,
    const encoder_measurement_t *current,
    const motion_control_tuning_t* tuning
)
{
    float inc1 = (current->channel1 - previous->channel1) * tuning->wheel_diameter / 360.0 * M_PI;
    float inc2 = (current->channel2 - previous->channel2) * tuning->wheel_diameter / 360.0 * M_PI;

    current_pose->x += (inc2 - inc1) * cos(current_pose->theta * M_PI / 180.0) / 2.0;
    current_pose->y += (inc2 - inc1) * sin(current_pose->theta * M_PI / 180.0) / 2.0;
    current_pose->theta -= (inc2 + inc1) / tuning->axle_width * 180.0 / M_PI;
}

