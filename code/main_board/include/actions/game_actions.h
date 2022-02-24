#pragma once

#include <stdbool.h>

#define POSE_DATA_TYPING \
    X_FLOAT_DATA(x) \
    X_FLOAT_DATA(y) \
    X_FLOAT_DATA(theta)

#define PUMP_DATA_TYPING \
    X_BOOL_DATA(value)

#define STEPPER_DATA_TYPING \
    X_INT_DATA(position) \
    X_FLOAT_DATA(speed) \
    X_FLOAT_DATA(acceleration)

#define DEFINE_GAME_VARIABLES \
    X(pose, game_action_set_pose, game_action_get_pose, POSE_DATA_TYPING) \
    X(pump, game_action_set_pump, game_action_get_pump, PUMP_DATA_TYPING) \
    X(stepper, game_action_set_stepper, game_action_get_stepper, STEPPER_DATA_TYPING)

// Define data parameter input structures
#define GAME_ACTION_STRUCT_NAME(variable_name) game_action_##variable_name##_data_t
#define X(variable_name, setter_function, getter_function, DATA_TYPING) \
struct GAME_ACTION_STRUCT_NAME(variable_name) { \
    DATA_TYPING \
};
#define X_FLOAT_DATA(x) float x;
#define X_INT_DATA(x) int x;
#define X_BOOL_DATA(x) bool x;
DEFINE_GAME_VARIABLES
#undef X
#undef X_FLOAT_DATA
#undef X_INT_DATA
#undef X_BOOL_DATA

int game_action_set_pose(int id, struct game_action_pose_data_t data);
int game_action_get_pose(int id, struct game_action_pose_data_t *data);
int game_action_set_pump(int id, struct game_action_pump_data_t data);
int game_action_get_pump(int id, struct game_action_pump_data_t *data);
int game_action_set_stepper(int id, struct game_action_stepper_data_t data);
int game_action_get_stepper(int id, struct game_action_stepper_data_t *data);
