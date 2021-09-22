#include "motion_control.h"
#include "i2c_master.h"

#define MOTOR_I2C_ADDR 0x0A
#define SQRT_3_2 0.86602540378
#define PI_2 6.28318530718
#define TICKS_PER_TURN (360*2)
#define ALLOWED_ERROR 100.0
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define abs(x) (((x) > 0.0) ? (x) : -(x))

typedef struct {
    float x;
    float y;
    float t;
} pose_t;

typedef struct {
    int16_t c1;
    int16_t c2;
    int16_t c3;
} motor_duty_cycle_t;

typedef struct {
    int16_t c1;
    int16_t c2;
    int16_t c3;
} encoder_t;

static pose_t current_pose, target_pose, last_pose, cumulated_pose_error;
static feedback_params_t pid = {0, 0, 0};
static wheel_geometry_t wheel_geometry;
static int is_running = 0;
static encoder_t prev_encoder;

static void encoder_delta_to_position(int16_t channel1, int16_t channel2, int16_t channel3, pose_t *pose);
static void position_setpoint_to_motor(float x_setpoint, float y_setpoint, float t_setpoint, motor_duty_cycle_t *motor);
static void write_motor_speed(motor_duty_cycle_t motor);
static void read_encoders(encoder_t *encoder);

void init_motion_control(feedback_params_t _pid, wheel_geometry_t _geometry)
{
    current_pose.x = 0;
    current_pose.y = 0;
    current_pose.t = 0;
    is_running = 0;

    motor_duty_cycle_t motor = {0, 0, 0};
    write_motor_speed(motor);

    pid.p = _pid.p;
    pid.i = _pid.i;
    pid.d = _pid.d;

    wheel_geometry.robot_radius = _geometry.robot_radius;
    wheel_geometry.wheel_radius = _geometry.wheel_radius;
}

void set_motion_target(float target_x, float target_y, float target_theta)
{
    read_encoders(&prev_encoder);

    target_pose.x = target_x * TICKS_PER_TURN / PI_2 / wheel_geometry.wheel_radius;
    target_pose.y = target_y * TICKS_PER_TURN / PI_2 / wheel_geometry.wheel_radius;
    target_pose.t = target_theta * TICKS_PER_TURN / PI_2 / wheel_geometry.wheel_radius;

    last_pose.x = current_pose.x;
    last_pose.x = current_pose.y;
    last_pose.x = current_pose.t;

    cumulated_pose_error.x = 0.0;
    cumulated_pose_error.y = 0.0;
    cumulated_pose_error.t = 0.0;

    is_running = 1;
}

void motion_control_loop(void)
{
    // Read encoder and update x, y, theta position
    encoder_t current_encoder;
    read_encoders(&current_encoder);
    encoder_delta_to_position(
        current_encoder.c1 - prev_encoder.c1, current_encoder.c2 - prev_encoder.c2, current_encoder.c3 - prev_encoder.c3,
        &current_pose
    );
    prev_encoder.c1 = current_encoder.c1;
    prev_encoder.c2 = current_encoder.c2;
    prev_encoder.c3 = current_encoder.c3;

    cumulated_pose_error.x += (target_pose.x - current_pose.x) * 10;
    cumulated_pose_error.y += (target_pose.y - current_pose.y) * 10;
    cumulated_pose_error.t += (target_pose.t - current_pose.t) * 10; // FIXME: Measure time

    printf("Current encoder: %d %d %d\n", current_encoder.c1, current_encoder.c2, current_encoder.c3);
    printf("Current position: %f %f %f\n", current_pose.x, current_pose.y, current_pose.t);
    printf("Current target: %f %f %f\n", target_pose.x, target_pose.y, target_pose.t);

    if ((abs(target_pose.x - current_pose.x) < ALLOWED_ERROR) && // FIXME: read speed
        (abs(target_pose.y - current_pose.y) < ALLOWED_ERROR) &&
        (abs(target_pose.t - current_pose.t) < ALLOWED_ERROR)) {
        printf("Arrived! %f %f\n", abs(target_pose.x - current_pose.x), ALLOWED_ERROR);
        is_running = 0;
    }

    if (!is_running) {
        motor_duty_cycle_t motor = {0, 0, 0};
        write_motor_speed(motor);
        return;
    }

    float x_setpoint = (target_pose.x - current_pose.x) * pid.p
        + cumulated_pose_error.x * pid.i
        + (current_pose.x - last_pose.x) / 10 * pid.d;
    float y_setpoint = (target_pose.y - current_pose.y) * pid.p
        + cumulated_pose_error.y * pid.i
        + (current_pose.y - last_pose.y) / 10 * pid.d;
    float t_setpoint = (target_pose.t - current_pose.t) * pid.p
        + cumulated_pose_error.t * pid.i
        + (current_pose.t - last_pose.t) / 10 * pid.d;

    last_pose.x = current_pose.x;
    last_pose.y = current_pose.y;
    last_pose.t = current_pose.t;

    printf("Setpoint error: %f %f %f\n", x_setpoint, y_setpoint, t_setpoint);

    motor_duty_cycle_t motor;
    position_setpoint_to_motor(x_setpoint, y_setpoint, t_setpoint, &motor);
    printf("Motor speed: %d %d %d\n", motor.c1, motor.c2, motor.c3);
    write_motor_speed(motor);
}

void stop_motion(void)
{
    is_running = 0;
    motor_duty_cycle_t motor = {0, 0, 0};
    write_motor_speed(motor);
}

int is_motion_done(void)
{
    return !is_running;
}

static void encoder_delta_to_position(int16_t channel1, int16_t channel2, int16_t channel3, pose_t *pose)
{
    pose->x += (channel3 - channel2) * SQRT_3_2;
    pose->y += channel1 - (channel3 + channel2) * 0.5;
    pose->t += channel1 + channel2 + channel3;
}

static void position_setpoint_to_motor(float x_setpoint, float y_setpoint, float t_setpoint, motor_duty_cycle_t *motor)
{
    float c1 = 2.0 * y_setpoint + t_setpoint;
    float c3 = x_setpoint / SQRT_3_2 / 2.0 + c1 - y_setpoint;
    float c2 = c3 - x_setpoint / SQRT_3_2;

    float max_value = max(abs(c1), max(abs(c2), abs(c3)));
    if (max_value >= 1.0) {
        c1 /= max_value;
        c2 /= max_value;
        c3 /= max_value;
    }

    motor->c1 = (int16_t)(c1 * INT16_MAX * 0.9);
    motor->c2 = (int16_t)(c2 * INT16_MAX * 0.9);
    motor->c3 = (int16_t)(c3 * INT16_MAX * 0.9);
}

static void write_motor_speed(motor_duty_cycle_t motor)
{
    send_i2c_command(MOTOR_I2C_ADDR, "Bhhh", 0x01, motor.c1, motor.c2, motor.c3);
}

static void read_encoders(encoder_t *encoder)
{
    send_i2c_command(MOTOR_I2C_ADDR, "B", 0x02);
    receive_from_i2c(MOTOR_I2C_ADDR, "hhh", &encoder->c1, &encoder->c2, &encoder->c3);
}