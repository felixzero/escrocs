#pragma once

#include <inttypes.h>

typedef float position_t;

void init_holonomic_feedback(position_t initial_x, position_t initial_y, position_t initial_theta);
void set_holonomic_feedback_target(position_t x, position_t y, position_t theta);
void holonomic_feedback_loop(void);
void holonomic_stop(void);
void get_holonomic_position(position_t *x, position_t *y, position_t *theta);
void set_holonomic_pid_parameters(float p, float i, float d);
