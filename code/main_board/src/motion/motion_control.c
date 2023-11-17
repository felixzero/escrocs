#include "motion_control.h"
#include "holonomic_wheel_base.h"

#include "../peripherals/motor_board_v3.h"
#include "system/task_priority.h"
#include "peripherals/ultrasonic_board.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <math.h>
#include <string.h>

#define TAG "Motion control"

#define MOTOR_DISABLING_TIMEOUT (100000 / portTICK_PERIOD_MS)

static bool reversed_side;

static TaskHandle_t motor_disabler_task_handle;
static QueueHandle_t input_target_queue, overwrite_pose_queue, output_status_queue;

static void motor_disabler_task(void *parameters);
static void motion_control_task(void *parameters);
static pose_t apply_reverse_transformation(const pose_t *pose, bool reversed_side);

void init_motion_control(bool reversed)
{
    disable_motors();
    write_motor_speed_raw(0.0, 0.0, 0.0);
    reversed_side = reversed;

    // Create FreeRTOS task
    TaskHandle_t task;
    input_target_queue = xQueueCreate(1, sizeof(motion_status_t));
    overwrite_pose_queue = xQueueCreate(1, sizeof(pose_t));
    output_status_queue = xQueueCreate(1, sizeof(motion_status_t));
    xTaskCreatePinnedToCore(motion_control_task, "motion_control", TASK_STACK_SIZE, NULL, MOTION_CONTROL_PRIORITY, &task, TIME_CRITICAL_CORE);

    xTaskCreatePinnedToCore(
        motor_disabler_task,
        "motor_disabler",
        TASK_STACK_SIZE,
        NULL,
        DISABLE_MOTOR_PRIORITY,
        &motor_disabler_task_handle,
        TIME_CRITICAL_CORE
    );
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

    motion_target.pose.x = isnan(target->x) ? current_pose.x : target->x;
    motion_target.pose.y = isnan(target->y) ? current_pose.y : target->y;
    motion_target.pose.theta = isnan(target->theta) ? current_pose.theta : target->theta;

    motion_target.motion_step = MOTION_STEP_RUNNING;

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
    encoder_measurement_t previous_encoder = { 0 };

    holonomic_wheel_base_set_values(&tuning);

    while (read_encoders(&previous_encoder) != ESP_OK) {
        ESP_LOGW(TAG, "Error while reading encoders; retrying...");
    }
    struct motion_data_t motion_data;
    motion_data.tuning = &tuning;
    
    int iteration = 0;

    while (true) {
        vTaskDelayUntil(&iteration_time, MOTION_PERIOD_MS / portTICK_PERIOD_MS);

        // Retrieve latest parameters
        xQueueReceive(overwrite_pose_queue, &current_pose, 0);
        if (xQueueReceive(input_target_queue, &motion_target, 0)) {
            motion_data.elapsed_time_ms = 0;
        }
        
        // Update pose according to encoders; in case of communication failure, keep previous values
        encoder_measurement_t encoder;
        memcpy(&encoder, &previous_encoder, sizeof(encoder));
        read_encoders(&encoder);
        holonomic_wheel_base_update_pose(&motion_data, &current_pose, &previous_encoder, &encoder);
        previous_encoder = encoder;

        if (iteration % 10 == 0) {
            ESP_LOGI(TAG, "Encoders: %f %f %f", encoder.channel1, encoder.channel2, encoder.channel3);
            ESP_LOGI(TAG, "Pose: %f %f %f", current_pose.x, current_pose.y, current_pose.theta);
        }

        float min_scanning_angle, max_scanning_angle;
        bool needs_detection;
        holonomic_wheel_base_get_detection_scanning_angles(
            &motion_data, &motion_target, &current_pose,
            &min_scanning_angle, &max_scanning_angle, &needs_detection
        );

        // FIXME
        //set_ultrasonic_scan_angle(0, 2 * M_PI);
        //ultrasonic_perform_scan();
        
        /*if (motion_target.perform_detection && needs_detection) {
            set_ultrasonic_scan_angle(min_scanning_angle, max_scanning_angle);
        } else {
            disable_ultrasonic_detection();
        }*/

        //bool obstacle_detected = ultrasonic_has_obstacle() && needs_detection && motion_target.perform_detection;
        bool obstacle_detected = false;
        // Calculate new motor targets
        if ((motion_target.motion_step == MOTION_STEP_DONE) || obstacle_detected) {
            write_motor_speed_raw(0.0, 0.0, 0.0);
        } else {
            enable_motors_and_set_timer();
            holonomic_wheel_base_apply_speed_to_motors(&motion_data, &motion_target, &current_pose);
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

static void motor_disabler_task(void *parameters)
{
    while (true) {
        if (!ulTaskNotifyTake(true, MOTOR_DISABLING_TIMEOUT)) {
            disable_motors();
        }
        vTaskDelay(1);
    }
}

void enable_motors_and_set_timer(void)
{
    enable_motors();
    xTaskNotifyGive(motor_disabler_task_handle);
}
