#include "motion_control.h"
#include "holonomic_wheel_base.h"

#include "../peripherals/motor_board_v3.h"
#include "system/task_priority.h"
#include "controllers/us_control.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <math.h>
#include <string.h>

#define TAG "Motion control"

#define MOTOR_DISABLING_TIMEOUT             pdMS_TO_TICKS(5000)
#define NUMBER_OF_CLEAR_ULTRASONIC_SCANS    5

static bool reversed_side;

static TaskHandle_t motor_disabler_task_handle;
static QueueHandle_t input_target_queue, overwrite_pose_queue, output_status_queue;

static void motor_disabler_task(void *parameters);
static void motion_control_task(void *parameters);
static pose_t apply_reverse_transformation(const pose_t *pose, bool reversed_side);

esp_err_t init_motion_control(bool reversed)
{
    esp_err_t err;

    err = disable_motors();
    if (err) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }

    err = write_motor_speed_rad_s(0.0, 0.0, 0.0);
    if (err) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }
    reversed_side = reversed;

    // Create FreeRTOS task
    TaskHandle_t task;
    input_target_queue = xQueueCreate(1, sizeof(motion_status_t));
    overwrite_pose_queue = xQueueCreate(1, sizeof(pose_t));
    output_status_queue = xQueueCreate(1, sizeof(motion_status_t));

    xTaskCreatePinnedToCore(
        motor_disabler_task,
        "motor_disabler",
        TASK_STACK_SIZE,
        NULL,
        DISABLE_MOTOR_PRIORITY,
        &motor_disabler_task_handle,
        LOW_CRITICITY_CORE
    );
    xTaskCreatePinnedToCore(motion_control_task, "motion_control", TASK_STACK_SIZE, NULL, MOTION_CONTROL_PRIORITY, &task, TIME_CRITICAL_CORE);

    return ESP_OK;
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
    current_pose = apply_reverse_transformation(&current_pose, reversed_side);

    motion_target.pose.x = isnan(target->x) ? current_pose.x : target->x;
    motion_target.pose.y = isnan(target->y) ? current_pose.y : target->y;
    motion_target.pose.theta = isnan(target->theta) ? current_pose.theta : target->theta;
    motion_target.pose = apply_reverse_transformation(&motion_target.pose, reversed_side);

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

bool is_blocked(void)
{
    motion_status_t status;
    xQueuePeek(output_status_queue, &status, 0);
    return status.is_blocked;
}

static void motion_control_task(void *parameters)
{
    pose_t current_pose = {
            .x = 0.0,
            .y = 0.0,
            .theta = 0.0
        };
    current_pose = apply_reverse_transformation(&current_pose, reversed_side);
    motion_status_t motion_target = {
        .motion_step = MOTION_STEP_DONE,
        .is_blocked = false,
    };
    motion_control_tuning_t tuning;
    holonomic_wheel_base_set_values(&tuning);

    struct motion_data_t motion_data = {
        .tuning = &tuning,
        .previous_speed = { 0 },
        .previous_time = 0,
        .please_stop = false,
    };

    int number_of_clear_ultrasonic_iterations_before_movement = 0;

    TickType_t iteration_time = xTaskGetTickCount();    
    for (int iteration = 0; true; ++iteration) {
        xTaskDelayUntil(&iteration_time, pdMS_TO_TICKS(MOTION_PERIOD_MS));

        // Retrieve latest parameters
        xQueueReceive(overwrite_pose_queue, &current_pose, 0);
        int previous_step = motion_target.motion_step;
        if (xQueueReceive(input_target_queue, &motion_target, 0)) {
            if (previous_step == MOTION_STEP_DONE) {
                motion_data.previous_time = esp_timer_get_time();
            }
            motion_data.please_stop = motion_target.motion_step == MOTION_STEP_DONE;
            motion_target.motion_step = MOTION_STEP_RUNNING;
        }
        
        // Update pose according to encoders
        encoder_measurement_t encoder_increment;
        if (read_encoder_increment(&encoder_increment) == ESP_OK) {
            holonomic_wheel_base_update_pose(&motion_data, &current_pose, &encoder_increment);
        }

        if (iteration % 10 == 0) {
            ESP_LOGI(TAG, "Encoders: %f %f %f", encoder_increment.channel1, encoder_increment.channel2, encoder_increment.channel3);
            ESP_LOGI(TAG, "Pose: %f %f %f", current_pose.x, current_pose.y, current_pose.theta);

        }

        bool has_obstacle = false;
        //Calculates a cone to scan to send to US board
        float center_scanning_angle, cone_scanning_angle;
        bool need_detection = false;
        scan_angle_t motion_cone;
        holonomic_wheel_base_get_detection_scanning_angles(
            &motion_data, &motion_target, &current_pose,
            &center_scanning_angle, &cone_scanning_angle, &need_detection
        );
        motion_cone.center_angle = center_scanning_angle;
        motion_cone.cone = cone_scanning_angle;

        if (motion_target.motion_step == MOTION_STEP_RUNNING && xQueueReceive(scan_over_queue, &has_obstacle, 0)) {
            //Perform obstacle detection logic
            if (need_detection && has_obstacle && motion_target.perform_detection) {
                number_of_clear_ultrasonic_iterations_before_movement = NUMBER_OF_CLEAR_ULTRASONIC_SCANS;
                ESP_LOGI(TAG, "Obstacle detected");
                motion_target.is_blocked = true;
            } else if (number_of_clear_ultrasonic_iterations_before_movement > 0) {
                number_of_clear_ultrasonic_iterations_before_movement--;
                ESP_LOGI(TAG, "undetected obstacle, waiting %d cycles before going", number_of_clear_ultrasonic_iterations_before_movement);
            }
        }
        xQueueOverwrite(motion_cone_queue, &motion_cone);

        // Calculate new motor targets
        if (motion_target.motion_step == MOTION_STEP_DONE) {
            write_motor_speed_rad_s(0.0, 0.0, 0.0);
        } else {
            enable_motors_and_set_timer();
            holonomic_wheel_base_apply_speed_to_motors(
                &motion_data,
                &motion_target,
                &current_pose,
                number_of_clear_ultrasonic_iterations_before_movement > 0
            );
        }

        motion_target.is_blocked = (number_of_clear_ultrasonic_iterations_before_movement > 0);
        // Broadcast current pose
        motion_status_t status;
        status.pose = current_pose;
        status.motion_step = motion_target.motion_step;
        xQueueOverwrite(output_status_queue, &status);
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
        uint32_t notified_value;
        if (!xTaskNotifyWait(0, ULONG_MAX, &notified_value, MOTOR_DISABLING_TIMEOUT)) {
            ESP_LOGI(TAG, "Disabling stepper motors");
            //disable_motors();
            //ESP_LOGI(TAG, "Enabled status: %d", are_motors_enabled());
        }

        vTaskDelay(10);
    }
}

void enable_motors_and_set_timer(void)
{
    enable_motors();
    xTaskNotify(motor_disabler_task_handle, 1, eSetValueWithoutOverwrite);
}
