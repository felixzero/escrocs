#pragma once

#include <stdbool.h>

#define SET_POSE_ARGUMENTS \
    X_FLOAT_ARGS(x) \
    X_FLOAT_ARGS(y) \
    X_FLOAT_ARGS(theta) \
    X_BOOL_ARGS(perform_detection)
#define SET_POSE_OUTPUT // nothing
#define GET_POSE_ARGUMENTS // nothing
#define GET_POSE_OUTPUT \
    X_FLOAT_OUTPUT(x) \
    X_FLOAT_OUTPUT(y) \
    X_FLOAT_OUTPUT(theta)
#define OVERWRITE_POSE_ARGUMENTS \
    X_FLOAT_ARGS(x) \
    X_FLOAT_ARGS(y) \
    X_FLOAT_ARGS(theta)
#define OVERWRITE_POSE_OUTPUT // nothing
#define STOP_MOTION_ARGUMENTS // nothing
#define STOP_MOTION_OUTPUT // nothing
#define IS_MOTION_DONE_ARGUMENTS // nothing
#define IS_MOTION_DONE_OUTPUT \
    X_BOOL_OUTPUT(motion_done)
#define SET_PUMP_ARGUMENTS \
    X_INT_ARGS(channel) \
    X_BOOL_ARGS(value)
#define SET_PUMP_OUTPUT // nothing
#define MOVE_STEPPER_ARGUMENTS \
    X_INT_ARGS(channel) \
    X_INT_ARGS(target) \
    X_FLOAT_ARGS(speed)
#define MOVE_STEPPER_OUTPUT // nothing
#define RESET_STEPPER_ARGUMENTS \
    X_INT_ARGS(channel) \
    X_INT_ARGS(value)
#define RESET_STEPPER_OUTPUT // nothing
#define GET_BUTTON_ARGUMENTS \
    X_INT_ARGS(channel)
#define GET_BUTTON_OUTPUT \
    X_BOOL_OUTPUT(status)

#define DEFINE_GAME_ACTION_FUNCTIONS \
    X(set_pose, game_action_set_pose, SET_POSE_ARGUMENTS, SET_POSE_OUTPUT) \
    X(get_pose, game_action_get_pose, GET_POSE_ARGUMENTS, GET_POSE_OUTPUT) \
    X(overwrite_pose, game_action_overwrite_pose, OVERWRITE_POSE_ARGUMENTS, OVERWRITE_POSE_OUTPUT) \
    X(stop_motion, game_action_stop_motion, STOP_MOTION_ARGUMENTS, STOP_MOTION_OUTPUT) \
    X(is_motion_done, game_action_is_motion_done, IS_MOTION_DONE_ARGUMENTS, IS_MOTION_DONE_OUTPUT) \
    X(set_pump, game_action_set_pump, SET_PUMP_ARGUMENTS, SET_PUMP_OUTPUT) \
    X(move_stepper, game_action_move_stepper, MOVE_STEPPER_ARGUMENTS, MOVE_STEPPER_OUTPUT) \
    X(reset_stepper, game_action_reset_stepper, RESET_STEPPER_ARGUMENTS, RESET_STEPPER_OUTPUT) \
    X(get_button, game_action_get_button, GET_BUTTON_ARGUMENTS, GET_BUTTON_OUTPUT)

// Define data parameter structures
#define GAME_ACTION_ARGUMENTS_STRUCT_NAME(action_name) game_action_##action_name##_input_data_t
#define GAME_ACTION_OUTPUT_STRUCT_NAME(action_name) game_action_##action_name##_output_data_t

#define X(action_name, function, ARGUMENTS, OUTPUT) \
struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(action_name) { \
    ARGUMENTS \
}; \
 \
struct GAME_ACTION_OUTPUT_STRUCT_NAME(action_name) { \
    OUTPUT \
};
#define X_FLOAT_ARGS(x) float x;
#define X_INT_ARGS(x) int x;
#define X_BOOL_ARGS(x) bool x;
#define X_FLOAT_OUTPUT(x) float x;
#define X_INT_OUTPUT(x) int x;
#define X_BOOL_OUTPUT(x) bool x;
DEFINE_GAME_ACTION_FUNCTIONS
#undef X
#undef X_FLOAT_ARGS
#undef X_INT_ARGS
#undef X_BOOL_ARGS
#undef X_FLOAT_OUTPUT
#undef X_INT_OUTPUT
#undef X_BOOL_OUTPUT

// Define function prototypes
#define X(action_name, function, ARGUMENTS, OUTPUT) \
struct GAME_ACTION_OUTPUT_STRUCT_NAME(action_name) game_action_ ## action_name(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(action_name) args);
DEFINE_GAME_ACTION_FUNCTIONS
#undef X

