#include "motion/motion_control.h"
#include "system/i2c_master.h"
#include "system/task_priority.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <math.h>

#define WHEEL_DIAMETER 100.0 // in mm
#define AXLE_WIDTH 294.8 // in mm
#define MOTION_PERIOD_MS 100
#define TICK_PER_DEGREE 1.0

#define TAG "Motion control"
#define MOTOR_I2C_ADDR 0x0A

#define CLAMP_ABS(x, clamp) ((fabs(x) > (clamp)) ? (clamp) * (x) / fabs(x) : (x))

static QueueHandle_t set_target_queue, motion_information_queue;

static void motion_control_task(void *parameters);
static void write_motor_speed(float speed1, float speed2, float speed3);
static void read_encoders(float *channel1, float *channel2, float *channel3);

typedef struct {
    float x;
    float y;
    float theta;
} position_t;

typedef struct {
    position_t position;
    bool motion_enabled;
} motion_target_t;

typedef struct {
    position_t current_position;
    bool motion_done;
} motion_information_t;

void init_motion_control(void)
{
    set_target_queue = xQueueCreate(1, sizeof(motion_target_t));
    motion_information_queue = xQueueCreate(1, sizeof(motion_information_t));

    TaskHandle_t task;
    xTaskCreate(motion_control_task, "motor_board", TASK_STACK_SIZE, NULL, MOTION_CONTROL_PRIORITY, &task);
}

void set_motion_target(float target_x, float target_y, float target_theta)
{
    motion_target_t motion_target;
    motion_target.motion_enabled = true;
    motion_target.position.x = target_x;
    motion_target.position.y = target_y;
    motion_target.position.theta = target_theta;

    xQueueOverwrite(set_target_queue, &motion_target);
}

void get_current_position(float *position_x, float *position_y, float *position_theta)
{
    motion_information_t information;
    xQueuePeek(motion_information_queue, &information, 0);
    
    *position_x = information.current_position.x;
    *position_y = information.current_position.y;
    *position_theta = information.current_position.theta;
}

void stop_motion(void)
{
    motion_target_t motion_target;
    motion_target.motion_enabled = false;

    xQueueOverwrite(set_target_queue, &motion_target);
}

bool is_motion_done(void)
{
    motion_information_t information;
    xQueuePeek(motion_information_queue, &information, 0);
    
    return information.motion_done;
}

static void motion_control_task(void *parameters)
{
    TickType_t iteration_time = xTaskGetTickCount();
    position_t current_position = { .x = 0.0, .y = 0.0, .theta = 0.0 };
    motion_target_t motion_target;

    while (true) {
        vTaskDelayUntil(&iteration_time, MOTION_PERIOD_MS / portTICK_PERIOD_MS);

        // Retrieve latest target
        motion_target_t motion_target;
        xQueueReceive(set_target_queue, &motion_target, 0);

        // If motion is halted, stop motors
        if (!motion_target.motion_enabled) {
            ESP_LOGI(TAG, "Motion stopped");
            //set_speed_targets(0.0, 0.0, 0.0);
            continue;
        }

        // Broadcast current position
        motion_information_t information;
        information.current_position = current_position;
        information.motion_done = false;
        xQueueOverwrite(motion_information_queue, &information);
    }
}

static void read_encoders(float *channel1, float *channel2, float *channel3)
{
    uint8_t reg = 0x02;
    int16_t encoder_raw_values[6];

    send_to_i2c(I2C_PORT_MOTOR, MOTOR_I2C_ADDR, &reg, 1);
    request_from_i2c(I2C_PORT_MOTOR, MOTOR_I2C_ADDR, encoder_raw_values, sizeof(encoder_raw_values));

    *channel1 = (float)encoder_raw_values[0] / TICK_PER_DEGREE;
    *channel2 = (float)encoder_raw_values[1] / TICK_PER_DEGREE;
    *channel3 = (float)encoder_raw_values[2] / TICK_PER_DEGREE;
}

static void write_motor_speed(float speed1, float speed2, float speed3)
{
    int8_t buffer[7];
    buffer[0] = 0x01;
    *((int16_t*)&buffer[1]) = 0.9 * CLAMP_ABS(speed1, 1.0) * INT16_MAX;
    *((int16_t*)&buffer[3]) = 0.9 * CLAMP_ABS(speed2, 1.0) * INT16_MAX;
    *((int16_t*)&buffer[5]) = 0.9 * CLAMP_ABS(speed3, 1.0) * INT16_MAX;
    send_to_i2c(I2C_PORT_MOTOR, MOTOR_I2C_ADDR, &buffer, sizeof(buffer));
}
