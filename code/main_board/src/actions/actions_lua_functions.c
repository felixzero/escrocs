#include "actions/game_actions.h"
#include "actions/actions_lua_functions.h"

#define X(action_name, function, ARGUMENTS, OUTPUT) \
static int game_action_ ## action_name ## _lua(lua_State *L) \
{ \
    int arg_id = 1; \
    (void)arg_id; \
    struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(action_name) args; \
ARGUMENTS \
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(action_name) output = game_action_ ## action_name(args); \
    (void)output; \
    int number_of_return_values = 0; \
OUTPUT \
    return number_of_return_values; \
}

#define X_FLOAT_ARGS(parameter_name) \
    if (!lua_isnumber(L, arg_id)) { \
        lua_pushstring(L, "Invalid argument: not a valid number for " # parameter_name); \
        lua_error(L); \
    } \
    args.parameter_name = lua_tonumber(L, arg_id++);
#define X_INT_ARGS(parameter_name) \
    if (!lua_isinteger(L, arg_id)) { \
        lua_pushstring(L, "Invalid argument: not a valid integer for " # parameter_name); \
        lua_error(L); \
    } \
    args.parameter_name = lua_tointeger(L, arg_id++);
#define X_BOOL_ARGS(parameter_name) \
    if (!lua_isboolean(L, arg_id)) { \
        lua_pushstring(L, "Invalid argument: not a valid boolean for " # parameter_name); \
        lua_error(L); \
    } \
    args.parameter_name = lua_toboolean(L, arg_id++);

#define X_FLOAT_OUTPUT(parameter_name) \
    lua_pushnumber(L, output.parameter_name); \
    number_of_return_values++;
#define X_INT_OUTPUT(parameter_name) \
    lua_pushinteger(L, output.parameter_name); \
    number_of_return_values++;
#define X_BOOL_OUTPUT(parameter_name) \
    lua_pushboolean(L, output.parameter_name); \
    number_of_return_values++;

DEFINE_GAME_ACTION_FUNCTIONS
#undef X

void register_lua_action_functions(lua_State *L)
{
#define X(action_name, function, ARGUMENTS, OUTPUT) \
    lua_pushcfunction(L, game_action_ ## action_name ## _lua); \
    lua_setglobal(L, #action_name);
DEFINE_GAME_ACTION_FUNCTIONS
#undef X
}

