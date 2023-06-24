#include "motion_control.h"
#include "motion_control.impl.h"

#include "peripherals/motor_board.h"
#include "system/task_priority.h"
#include "peripherals/ultrasonic_board.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <math.h>

#define TAG "Motion control"

#define OBSTACLE_FRAMES_TO_CLEAR 10
#define MAP_LENGTH 300.0
#define MAX_LIDAR_REFINEMENT_XY 300.0
#define MAX_LIDAR_REFINEMENT_T 0.3

static QueueHandle_t input_target_queue, overwrite_pose_queue, output_status_queue, tuning_queue;

static bool reversed_side;

static void motion_control_task(void *parameters);
static pose_t apply_reverse_transformation(const pose_t *pose, bool reversed_side);

void init_motion_control(bool reversed)
{
    write_motor_speed(0.0, 0.0, 0.0);
    reversed_side = reversed;

    // Create FreeRTOS task
    TaskHandle_t task;
    input_target_queue = xQueueCreate(1, sizeof(motion_status_t));
    overwrite_pose_queue = xQueueCreate(1, sizeof(pose_t));
    output_status_queue = xQueueCreate(1, sizeof(motion_status_t));
    tuning_queue = xQueueCreate(1, sizeof(motion_control_tuning_t));
    xTaskCreate(motion_control_task, "motion_control", TASK_STACK_SIZE, NULL, MOTION_CONTROL_PRIORITY, &task);
}

pose_t get_current_pose(void)
{
    motion_status_t status;
    xQueuePeek(output_status_queue, &status, 0);
    return apply_reverse_transformation(&status.pose, reversed_side);
}

void set_motion_target(const pose_t *target, bool perform_detection)
{
    motion_status_t motion_target;
    motion_target.perform_detection = perform_detection;
    pose_t current_pose = get_current_pose();
    pose_t setpoint_pose = apply_reverse_transformation(target, reversed_side);
    motion_control_on_motion_target_set(&motion_target, &setpoint_pose, &current_pose);

    // Send request to task
    ESP_LOGI(TAG, "Setting target to: %f %f %f", target->x, target->y, target->theta);
    xQueueOverwrite(input_target_queue, &motion_target);
}

void overwrite_current_pose(const pose_t *target)
{
    pose_t known_pose = apply_reverse_transformation(target, reversed_side);
    xQueueOverwrite(overwrite_pose_queue, &known_pose);
}

void stop_motion(void)
{
    ESP_LOGI(TAG, "Stopping motion");
    motion_status_t target = {
        .motion_step = MOTION_STEP_DONE
    };
    xQueueOverwrite(input_target_queue, &target);
}

bool is_motion_done(void)
{
    motion_status_t status;
    xQueuePeek(output_status_queue, &status, 0);
    
    return status.motion_step == MOTION_STEP_DONE;
}

void set_motion_control_tuning(const motion_control_tuning_t *tuning)
{
    xQueueOverwrite(tuning_queue, tuning);
}

static void motion_control_task(void *parameters)
{
    TickType_t iteration_time = xTaskGetTickCount();
    pose_t current_pose = {
            .x = 0.0,
            .y = 0.0,
            .theta = 0.0
        };
    current_pose = apply_reverse_transformation(&current_pose, reversed_side);
    motion_status_t motion_target = {
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
    motion_control_on_init(&motion_data, &tuning);
    int iteration = 0;

    while (true) {
        vTaskDelayUntil(&iteration_time, MOTION_PERIOD_MS / portTICK_PERIOD_MS);

        // Retrieve latest parameters
        xQueueReceive(overwrite_pose_queue, &current_pose, 0);
        if (xQueueReceive(input_target_queue, &motion_target, 0)) {
            motion_control_on_new_target_received(motion_data, &current_pose);
        }
        if (xQueueReceive(tuning_queue, &tuning, 0)) {
            motion_control_on_tuning_updated(motion_data, &tuning);
        }
        
        // Update pose according to encoders
        encoder_measurement_t encoder;
        read_encoders(&encoder);
        motion_control_update_pose(motion_data, &current_pose, &previous_encoder, &encoder);
        previous_encoder = encoder;

        if (iteration % 100 == 0) {
            ESP_LOGI(TAG, "Pose: %f %f %f", current_pose.x, current_pose.y, current_pose.theta);
        }

        float min_scanning_angle, max_scanning_angle;
        bool needs_detection;
        motion_control_scanning_angles(
            motion_data, &motion_target, &current_pose,
            &min_scanning_angle, &max_scanning_angle, &needs_detection
        );
        
        if (motion_target.perform_detection && needs_detection) {
            set_ultrasonic_scan_angle(min_scanning_angle, max_scanning_angle);
        } else {
            disable_ultrasonic_detection();
        }

        bool obstacle_detected = ultrasonic_has_obstacle() && needs_detection && motion_target.perform_detection;
        // Calculate new motor targets
        if ((motion_target.motion_step == MOTION_STEP_DONE) || obstacle_detected) {
            write_motor_speed(0.0, 0.0, 0.0);
        } else {
            motion_control_on_motor_loop(motion_data, &motion_target, &current_pose);
        }

        // Broadcast current pose
        motion_status_t status;
        status.pose = current_pose;
        status.motion_step = motion_target.motion_step;
        xQueueOverwrite(output_status_queue, &status);

        iteration++;
    }
}

static pose_t apply_reverse_transformation(const pose_t *pose, bool reversed_side)
{
    pose_t reversed_pose;
    if (!reversed_side) {
        reversed_pose = *pose;
    } else {
        reversed_pose.x = -pose->x;
        reversed_pose.y = pose->y;
        reversed_pose.theta = fmodf(M_PI - pose->theta, 2.0 * M_PI);
    }
    return reversed_pose;
}
