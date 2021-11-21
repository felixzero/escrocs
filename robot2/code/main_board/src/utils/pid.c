#include "utils/pid.h"

void init_pid(pid_handle_t *handle, const pid_parameters_t *parameters)
{
    handle->parameters = *parameters;
    handle->cumulated_error = 0.0;
    handle->previous_error = 0.0;
}

float compute_pid_feedback(pid_handle_t *handle, float error, float elapsed_time)
{
    float error_derivative = (error - handle->previous_error) / elapsed_time;
    handle->cumulated_error += error * elapsed_time;
    handle->previous_error = error;

    return handle->parameters.p * error + handle->parameters.i * handle->cumulated_error + handle->parameters.d * error_derivative;
}

