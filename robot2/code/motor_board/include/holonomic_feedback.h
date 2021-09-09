#pragma once

void init_holonomic_feedback(float initial_x, float initial_y, float initial_theta);
void set_holonomic_feedback_target(float x, float y, float theta);
void holonomic_feedback_loop(void);

