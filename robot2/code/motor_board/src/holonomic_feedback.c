#include "holonomic_feedback.h"
#include "motor.h"
#include "encoder.h"

#include <Arduino.h>

#define SQRT_3_2 0.86602540378

// Current position
static position_t x = 0, y = 0, theta = 0;
static position_t target_x = 0, target_y = 0, target_theta = 0;
static float parameter_p = 0.001, parameter_i = 0.0, parameter_d = 0.0;
static char is_running = 0;

static void encoder_delta_to_position(int16_t channel1, int16_t channel2, int16_t channel3, position_t *x, position_t *y, position_t *theta);
static void position_setpoint_to_motor(position_t x_setpoint, position_t y_setpoint, position_t theta_setpoint, \
    motor_speed_t *channel1, motor_speed_t *channel2, motor_speed_t *channel3);

void init_holonomic_feedback(position_t initial_x, position_t initial_y, position_t initial_theta)
{
    init_encoders();
    init_motors();

    x = initial_x;
    y = initial_y;
    theta = initial_theta;
}

void set_holonomic_feedback_target(position_t x, position_t y, position_t theta)
{
    target_x = x;
    target_y = y;
    target_theta = theta;
    is_running = 1;
}

void holonomic_feedback_loop(void)
{
    if (!is_running) {
        return;
    }

    // Read encoder and update x, y, theta position
    static int16_t prev_channel1 = 0, prev_channel2 = 0, prev_channel3 = 0;
    int16_t channel1, channel2, channel3;
    read_encoders(&channel1, &channel2, &channel3);
    encoder_delta_to_position(channel1 - prev_channel1, channel2 - prev_channel2, channel3 - prev_channel3, &x, &y, &theta);
    prev_channel1 = channel1;
    prev_channel2 = channel2;
    prev_channel3 = channel3;

    position_t x_setpoint = (target_x - x) * parameter_p;
    position_t y_setpoint = (target_y - y) * parameter_p;
    position_t theta_setpoint = (target_theta - theta) * parameter_p;

    motor_speed_t channel1_sp, channel2_sp, channel3_sp;
    position_setpoint_to_motor(x_setpoint, y_setpoint, theta_setpoint, &channel1_sp, &channel2_sp, &channel3_sp);

    write_motor_speed(channel1_sp, channel2_sp, channel3_sp);
}

static void encoder_delta_to_position(int16_t channel1, int16_t channel2, int16_t channel3, position_t *x, position_t *y, position_t *theta)
{
    *x += (channel3 - channel2) * SQRT_3_2;
    *y += (channel3 + channel2) * 0.5 - channel1;
    *theta -= channel1 + channel2 + channel3;
}

static void position_setpoint_to_motor(float x_setpoint, float y_setpoint, float theta_setpoint, \
    motor_speed_t *channel1, motor_speed_t *channel2, motor_speed_t *channel3)
{
    *channel1 = constrain((- 5.0 * theta_setpoint - 2.0 * y_setpoint) / 3.0, -1.0, 1.0) * 32767;
    *channel2 = constrain((y_setpoint - theta_setpoint - SQRT_3_2 * 2.0 * x_setpoint) / 3.0, -1.0, 1.0) * 32767;
    *channel3 = constrain((y_setpoint - theta_setpoint + SQRT_3_2 * 2.0 * x_setpoint) / 3.0, -1.0, 1.0) * 32767;
}

void holonomic_stop(void)
{
    is_running = 0;
}

void get_holonomic_position(position_t *x_return, position_t *y_return, position_t *theta_return)
{
    *x_return = x;
    *y_return = y;
    *theta_return = theta;
}

void set_holonomic_pid_parameters(float p, float i, float d)
{
    parameter_p = p;
    parameter_i = i;
    parameter_d = d;
}