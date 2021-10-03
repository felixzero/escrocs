#pragma once

/**
 * PID parameters of the control loop
 */
typedef struct {
    float p;
    float i;
    float d;
} feedback_params_t;

/**
 * Dimension of the robot
 * @param wheel_radius radius of the holonomic wheels in mm
 * @param robot_radius distance between wheels and the center of the robot in mm
 */
typedef struct {
    float wheel_radius;
    float robot_radius;
    float friction_coefficient;
    float max_speed;
} wheel_geometry_t;

/**
 * Initialize the motion control system
 */
void init_motion_control(feedback_params_t pid, wheel_geometry_t geometry);

/**
 * Set a (x, y, theta) target in (mm, mm, deg)
 */
void set_motion_target(float target_x, float target_y, float target_theta);

/**
 * Get current (x, y, theta) position
 */
void get_current_position(float *position_x, float *position_y, float *position_theta);

/**
 * Stop all motion
 */
void stop_motion(void);

/**
 * Check if the robot is still in motion
 */
int is_motion_done(void);

