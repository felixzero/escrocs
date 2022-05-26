#include "motion/motion_control.h"
#include "motion/motion_control.impl.h"

#include "motion/motor_board.h"
#include "system/task_priority.h"
#include "peripherals/lidar_board.h"

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
static QueueHandle_t input_lidar_queue, output_pose_lidar_queue, output_obstacle_lidar_queue;

static bool reversed_side;

static void motion_control_task(void *parameters);
static void lidar_communication_task(void *args);
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
    input_lidar_queue = xQueueCreate(1, sizeof(pose_t));
    output_pose_lidar_queue = xQueueCreate(1, sizeof(pose_t));
    output_obstacle_lidar_queue = xQueueCreate(1, sizeof(float*));
    xTaskCreate(lidar_communication_task, "lidar_communication", TASK_STACK_SIZE, NULL, LIDAR_COMMUNICATION_PRIORITY, &task);
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
    unsigned int obstacle_frames = 0, clear_frames = 0;

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
        xQueueOverwrite(input_lidar_queue, &current_pose);

        if (iteration % 100 == 0) {
            ESP_LOGI(TAG, "Pose: %f %f %f", current_pose.x, current_pose.y, current_pose.theta);
        }

        // Retrieve absolute pose from lidar
        /*pose_t lidar_pose;
        if (xQueueReceive(output_pose_lidar_queue, &lidar_pose, 0)) {
            if (
                (fabsf(current_pose.x - lidar_pose.x) < MAX_LIDAR_REFINEMENT_XY)
                && (fabsf(current_pose.y - lidar_pose.y) < MAX_LIDAR_REFINEMENT_XY)
                && (fabsf(current_pose.theta - lidar_pose.theta) < MAX_LIDAR_REFINEMENT_T)
            ) {
                ESP_LOGD(TAG, "Refining with pose %f %f %f", lidar_pose.x, lidar_pose.y, lidar_pose.theta);
                current_pose.x = tuning.lidar_filter * lidar_pose.x + (1.0 - tuning.lidar_filter) * current_pose.x;
                current_pose.y = tuning.lidar_filter * lidar_pose.y + (1.0 - tuning.lidar_filter) * current_pose.y;
                current_pose.theta = tuning.lidar_filter * lidar_pose.theta + (1.0 - tuning.lidar_filter) * current_pose.theta;
            } else {
                ESP_LOGW(TAG, "Proposed refinement from lidar too far: %f %f %f", lidar_pose.x, lidar_pose.y, lidar_pose.theta);
            }
        }*/

        float *obstacle_distances_by_angle;
        if (xQueueReceive(output_obstacle_lidar_queue, &obstacle_distances_by_angle, 0)) {
            if (
                motion_target.perform_detection
                && motion_control_is_obstacle_near(motion_data, &motion_target, &current_pose, obstacle_distances_by_angle)
            ) {
                if (clear_frames == 0) {
                    obstacle_frames = OBSTACLE_FRAMES_TO_CLEAR;
                    ESP_LOGW(TAG, "Warning: obstacle found");
                } else {
                    clear_frames = 1;
                }
            } else if (obstacle_frames > 0) {
                obstacle_frames--;
            }

            free(obstacle_distances_by_angle);
        }

        // Calculate new motor targets
        if ((motion_target.motion_step == MOTION_STEP_DONE) || (obstacle_frames > 0)) {
            write_motor_speed(0.0, 0.0, 0.0);
        } else {
            motion_control_on_motor_loop(motion_data, &motion_target, &current_pose);
            clear_frames = 1;
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

static void lidar_communication_task(void *args)
{
    TickType_t iteration_time = xTaskGetTickCount();

    for (int i = 0; true; i++) {
        pose_t current_pose, lidar_pose;

        vTaskDelayUntil(&iteration_time, LIDAR_PERIOD_MS / portTICK_PERIOD_MS);
        xQueueReceive(input_lidar_queue, &current_pose, portMAX_DELAY);

        float *obstacle_distances_by_angle = malloc(NUMBER_OF_CLUSTER_ANGLES * sizeof(float));
        if (!refine_pose(&current_pose, &lidar_pose, obstacle_distances_by_angle)) {
            free(obstacle_distances_by_angle);
            continue;
        }

        if (i % 20 == 0) {
            ESP_LOGI(
                TAG, "Lidar board refined position to (X=%f, Y=%f, T=%f)",
                lidar_pose.x, lidar_pose.y, lidar_pose.theta
            );
        }

        xQueueOverwrite(output_pose_lidar_queue, &lidar_pose);
        if (xQueueSend(output_obstacle_lidar_queue, &obstacle_distances_by_angle, 0) != pdTRUE) {
            free(obstacle_distances_by_angle);
        }
    }
}
