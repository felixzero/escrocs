#pragma once

typedef struct {
    float p;
    float i;
    float d;
} pid_parameters_t;

typedef struct {
    pid_parameters_t parameters;
    float cumulated_error;
    float previous_error;
} pid_handle_t;

void init_pid(pid_handle_t *handle, const pid_parameters_t *parameters);

float compute_pid_feedback(pid_handle_t *handle, float error, float elapsed_time);
