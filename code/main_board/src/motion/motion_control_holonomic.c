#include "motion/motion_control.h"

// This file is only compiled for holonomic robots
#ifdef CONFIG_ESP_ROBOT_HOLONOMIC

#include "motion/motor_board.h"
#include "system/task_priority.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <math.h>

#define PID_PERIOD 10
#define SQRT_3_2 0.86602540378
#define PI_2 6.28318530718
#define TICKS_PER_TURN 360
#define ALLOWED_ERROR_XY 5.0
#define ALLOWED_ERROR_T 0.2
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define abs(x) (((x) > 0.0) ? (x) : -(x))
#define sign(x) ((x == 0.0) ? 1.0 : ((x) / abs(x)))

#define TAG "Motion control"

typedef struct {
    pose_t current_pose;
    bool motion_done;
} motion_information_t;

static QueueHandle_t target_setting_queue, motion_information_queue, tuning_queue;

static void motion_control_task(void *parameters);
static void encoder_delta_to_position(
    const motion_control_tuning_t *tuning,
    float channel1,
    float channel2,
    float channel3,
    pose_t *pose
);
static float apply_friction_non_linearity(float setpoint, const motion_control_tuning_t *tuning);
static void position_setpoint_to_motor(
    const pose_t *current_pose,
    const motion_control_tuning_t *tuning,
    float x_setpoint,
    float y_setpoint,
    float t_setpoint,
    float motor_setpoints[]
);


void init_motion_control(void)
{
    write_motor_speed(0.0, 0.0, 0.0);

    // Create FreeRTOS task
    TaskHandle_t task;
    target_setting_queue = xQueueCreate(1, sizeof(pose_t));
    motion_information_queue = xQueueCreate(1, sizeof(motion_information_t));
    tuning_queue = xQueueCreate(1, sizeof(motion_control_tuning_t));
    xTaskCreate(motion_control_task, "motion_control", TASK_STACK_SIZE, NULL, MOTION_CONTROL_PRIORITY, &task);
}

void set_motion_target(const pose_t *target)
{
    // Send request to task
    ESP_LOGI(TAG, "Setting target to: %f %f %f", target->x, target->y, target->theta);
    xQueueOverwrite(target_setting_queue, target);
}

pose_t get_current_pose(void)
{
    motion_information_t information;
    xQueuePeek(motion_information_queue, &information, 0);
    return information.current_pose;
}

void stop_motion(void)
{

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

    pose_t current_pose = { .x = 0.0, .y = 0.0, .theta = 0.0 }, target_pose, last_pose, cumulated_pose_error;
    encoder_measurement_t prev_encoder;
    motion_control_tuning_t tuning;
    bool is_running = false;

    // Set default tuning parameters
    #define X(name, value) tuning.name = value;
    MOTION_CONTROL_TUNING_FIELDS
    #undef X

    while (1) {
        vTaskDelayUntil(&iteration_time, PID_PERIOD / portTICK_PERIOD_MS);

        // If a new target has been requested
        if (xQueueReceive(target_setting_queue, &target_pose, 0)) {
            ESP_LOGI(TAG, "Received new motion target.");
            read_encoders(&prev_encoder);

            last_pose.x = current_pose.x;
            last_pose.y = current_pose.y;
            last_pose.theta = current_pose.theta;

            cumulated_pose_error.x = 0.0;
            cumulated_pose_error.y = 0.0;
            cumulated_pose_error.theta = 0.0;

            is_running = true;
        }
        xQueueReceive(tuning_queue, &tuning, 0);

        // Read encoder and update x, y, theta position
        encoder_measurement_t current_encoder;
        read_encoders(&current_encoder);
        encoder_delta_to_position(
            &tuning,
            current_encoder.channel1 - prev_encoder.channel1,
            current_encoder.channel2 - prev_encoder.channel2,
            current_encoder.channel3 - prev_encoder.channel3,
            &current_pose
        );
        prev_encoder.channel1 = current_encoder.channel1;
        prev_encoder.channel2 = current_encoder.channel2;
        prev_encoder.channel3 = current_encoder.channel3;
        ESP_LOGD(TAG, "Current pose: X=%f Y=%f Theta=%f", current_pose.x, current_pose.y, current_pose.theta);

        cumulated_pose_error.x += (target_pose.x - current_pose.x) * PID_PERIOD;
        cumulated_pose_error.y += (target_pose.y - current_pose.y) * PID_PERIOD;
        cumulated_pose_error.theta += (target_pose.theta - current_pose.theta) * PID_PERIOD;

        float motion_x = (current_pose.x - last_pose.x) / PID_PERIOD;
        float motion_y = (current_pose.y - last_pose.y) / PID_PERIOD;
        float motion_theta = (current_pose.theta - last_pose.theta) / PID_PERIOD;

        // Broadcast current pose
        motion_information_t information;
        information.current_pose = current_pose;
        information.motion_done = !is_running;
        xQueueOverwrite(motion_information_queue, &information);

        if (!is_running) {
            ESP_LOGD(TAG, "Motor not running; skipping");
            write_motor_speed(0.0, 0.0, 0.0);
            continue;
        }

        if ((abs(target_pose.x - current_pose.x) < ALLOWED_ERROR_XY) &&
            (abs(target_pose.y - current_pose.y) < ALLOWED_ERROR_XY) &&
            (abs(target_pose.theta - current_pose.theta) < ALLOWED_ERROR_T)) {
            ESP_LOGI(TAG, "Motion finished");
            is_running = 0;
        }

        float x_setpoint = (target_pose.x - current_pose.x) * tuning.feedback_p
            + cumulated_pose_error.x * tuning.feedback_i
            + motion_x * tuning.feedback_d;
        float y_setpoint = (target_pose.y - current_pose.y) * tuning.feedback_p
            + cumulated_pose_error.y * tuning.feedback_i
            + motion_y * tuning.feedback_d;
        float t_setpoint = ((target_pose.theta - current_pose.theta) * tuning.feedback_p
            + cumulated_pose_error.theta * tuning.feedback_i
            + motion_theta * tuning.feedback_d)  / TICKS_PER_TURN * tuning.wheel_radius / tuning.robot_radius;

        last_pose.x = current_pose.x;
        last_pose.y = current_pose.y;
        last_pose.theta = current_pose.theta;

        float motor_setpoints[3];
        position_setpoint_to_motor(&current_pose, &tuning, x_setpoint, y_setpoint, t_setpoint, motor_setpoints);
        write_motor_speed(motor_setpoints[0], motor_setpoints[1], motor_setpoints[2]);
        ESP_LOGD(TAG, "Wrote motor speed %f %f %f", motor_setpoints[0], motor_setpoints[1], motor_setpoints[2]);
    }
}

static void encoder_delta_to_position(
    const motion_control_tuning_t *tuning,
    float channel1,
    float channel2,
    float channel3,
    pose_t *current_pose
)
{
    float x_ref_robot = (-channel2 + channel3) / SQRT_3_2 / 2.0;
    float y_ref_robot = (-2.0 * channel1 + channel2 + channel3) / 3.0;
    float t_ref_robot = (channel1 + channel2 + channel3) / 3.0;

    current_pose->x += x_ref_robot * cos(current_pose->theta) + y_ref_robot * sin(current_pose->theta);
    current_pose->y += - x_ref_robot * sin(current_pose->theta) + y_ref_robot * cos(current_pose->theta);
    current_pose->theta += t_ref_robot / TICKS_PER_TURN * tuning->wheel_radius / tuning->robot_radius;
}

static float apply_friction_non_linearity(float setpoint, const motion_control_tuning_t *tuning)
{
    float clamped = abs(setpoint) * (tuning->max_speed - tuning->friction_coefficient) + tuning->friction_coefficient;
    return clamped * sign(setpoint);
}

static void position_setpoint_to_motor(
    const pose_t *current_pose,
    const motion_control_tuning_t *tuning,
    float x_setpoint,
    float y_setpoint,
    float t_setpoint,
    float motor_setpoints[]
)
{
    float x_ref_robot = x_setpoint * cos(current_pose->theta) - y_setpoint * sin(current_pose->theta);
    float y_ref_robot = x_setpoint * sin(current_pose->theta) + y_setpoint * cos(current_pose->theta);
    float t_ref_robot = t_setpoint;

    float c1 = -y_ref_robot + t_ref_robot;
    float c2 = - SQRT_3_2 * x_ref_robot + y_ref_robot / 2.0 + t_ref_robot;
    float c3 = SQRT_3_2 * x_ref_robot + y_ref_robot / 2.0 + t_ref_robot;

    float max_value = max(abs(c1), max(abs(c2), abs(c3)));
    if (max_value >= 1.0) {
        c1 /= max_value;
        c2 /= max_value;
        c3 /= max_value;
    }

    motor_setpoints[0] = apply_friction_non_linearity(c1, tuning);
    motor_setpoints[1] = apply_friction_non_linearity(c2, tuning);
    motor_setpoints[2] = apply_friction_non_linearity(c3, tuning);
}

#endif // CONFIG_ESP_ROBOT_HOLONOMIC
