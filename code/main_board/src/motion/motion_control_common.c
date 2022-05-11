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

static QueueHandle_t input_target_queue, output_status_queue, tuning_queue;

static void motion_control_task(void *parameters);

void init_motion_control(void)
{
    write_motor_speed(0.0, 0.0, 0.0);

    // Create FreeRTOS task
    TaskHandle_t task;
    input_target_queue = xQueueCreate(1, sizeof(motion_status_t));
    output_status_queue = xQueueCreate(1, sizeof(motion_status_t));
    tuning_queue = xQueueCreate(1, sizeof(motion_control_tuning_t));
    xTaskCreate(motion_control_task, "motion_control", TASK_STACK_SIZE, NULL, MOTION_CONTROL_PRIORITY, &task);
}

pose_t get_current_pose(void)
{
    motion_status_t status;
    xQueuePeek(output_status_queue, &status, 0);
    return status.pose;
}

void set_motion_target(const pose_t *target)
{
    motion_status_t motion_target;
    pose_t current_pose = get_current_pose();
    motion_control_on_motion_target_set(&motion_target, target, &current_pose);

    // Send request to task
    ESP_LOGI(TAG, "Setting target to: %f %f %f", target->x, target->y, target->theta);
    xQueueOverwrite(input_target_queue, &motion_target);
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
        ESP_LOGD(TAG, "Pose: %f %f %f", current_pose.x, current_pose.y, current_pose.theta);

        // Retrieve absolute pose from lidar
        if (iteration % (LIDAR_PERIOD_MS / MOTION_PERIOD_MS) == 0) {
            pose_t lidar_pose;
            refine_pose(&current_pose, &lidar_pose);
            current_pose.x = tuning.lidar_filter * lidar_pose.x + (1.0 - tuning.lidar_filter) * current_pose.x;
            current_pose.y = tuning.lidar_filter * lidar_pose.y + (1.0 - tuning.lidar_filter) * current_pose.y;
            current_pose.theta = tuning.lidar_filter * lidar_pose.theta + (1.0 - tuning.lidar_filter) * current_pose.theta;
        }

        // Calculate new motor targets
        if (motion_target.motion_step == MOTION_STEP_DONE) {
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
