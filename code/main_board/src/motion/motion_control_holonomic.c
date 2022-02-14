#include "motion/motion_control.h"

// This file is only compiled for holonomic robots
#define CONFIG_ESP_ROBOT_HOLONOMIC
#ifdef CONFIG_ESP_ROBOT_HOLONOMIC

#include "motion/motor_board.h"
#include "system/task_priority.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <math.h>

#define MOTION_PERIOD_MS 10

#define TAG "Motion control"

static void motion_control_task(void *parameters);

void init_motion_control(void)
{
    write_motor_speed(0.0, 0.0, 0.0);
}

void set_motion_target(const pose_t *target)
{

}

pose_t get_current_pose(void)
{
    pose_t pose = {};
    encoder_measurement_t measurements;
    read_encoders(&measurements);
    pose.x = measurements.channel1;
    pose.y = measurements.channel2;
    pose.theta = measurements.channel3;
    return pose;
}

void stop_motion(void)
{

}

bool is_motion_done(void)
{
    return false;
}

void set_motion_control_tuning(const motion_control_tuning_t *tuning)
{

}

static void motion_control_task(void *parameters)
{
    
}

#endif // CONFIG_ESP_ROBOT_HOLONOMIC
