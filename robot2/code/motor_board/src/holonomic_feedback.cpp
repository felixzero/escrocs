#include "motor.h"
#include "encoder.h"

#include <Arduino.h>

#define SQRT_3_2 0.86602540378

// Current position
static float x = 0, y = 0, theta = 0;
static float target_x = 0, target_y = 0, target_theta = 0;

static void encoder_delta_to_position(int16_t channel1, int16_t channel2, int16_t channel3, float *x, float *y, float *theta);
static void position_setpoint_to_motor(float x_setpoint, float y_setpoint, float theta_setpoint, float *channel1, float *channel2, float *channel3);

void init_holonomic_feedback(float initial_x, float initial_y, float initial_theta)
{
    Serial.begin(9600);

    init_encoders();
    init_motors();

    x = initial_x;
    y = initial_y;
    theta = initial_theta;
}

void set_holonomic_feedback_target(float x, float y, float theta)
{
    target_x = x;
    target_y = y;
    target_theta = theta;
}

void holonomic_feedback_loop(void)
{
    // Read encoder and update x, y, theta position
    static int16_t prev_channel1 = 0, prev_channel2 = 0, prev_channel3 = 0;
    int16_t channel1, channel2, channel3;
    read_encoders(&channel1, &channel2, &channel3);
    encoder_delta_to_position(channel1 - prev_channel1, channel2 - prev_channel2, channel3 - prev_channel3, &x, &y, &theta);
    prev_channel1 = channel1;
    prev_channel2 = channel2;
    prev_channel3 = channel3;

    Serial.print("Position > ");
    Serial.print(x);
    Serial.print(" ");
    Serial.print(y);
    Serial.print(" ");
    Serial.println(theta);

    float x_setpoint = (target_x - x) / 1000.0;
    float y_setpoint = (target_y - y) / 1000.0;
    float theta_setpoint = (target_theta - theta) / 1000.0;
    float channel1_sp, channel2_sp, channel3_sp;
    position_setpoint_to_motor(x_setpoint, y_setpoint, theta_setpoint, &channel1_sp, &channel2_sp, &channel3_sp);

    write_motor_speed(channel1_sp, channel2_sp, channel3_sp);
}

static void encoder_delta_to_position(int16_t channel1, int16_t channel2, int16_t channel3, float *x, float *y, float *theta)
{
    *x += (channel3 - channel2) * SQRT_3_2;
    *y += (channel3 + channel2) * 0.5 - channel1;
    *theta -= channel1 + channel2 + channel3;
}

static void position_setpoint_to_motor(float x_setpoint, float y_setpoint, float theta_setpoint, float *channel1, float *channel2, float *channel3)
{
    *channel1 = (- 5.0 * theta_setpoint - 2.0 * y_setpoint) / 3.0;
    *channel2 = (y_setpoint - theta_setpoint - SQRT_3_2 * 2.0 * x_setpoint) / 3.0;
    *channel3 = (y_setpoint - theta_setpoint + SQRT_3_2 * 2.0 * x_setpoint) / 3.0;
}
