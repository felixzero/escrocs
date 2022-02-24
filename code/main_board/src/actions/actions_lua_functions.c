#include "actions/game_actions.h"
#include "actions/actions_lua_functions.h"

#define X(variable, setter, getter, DATA_TYPING) \
static int game_action_set_ ## variable ## _lua(lua_State *L) \
{ \
    int arg_id = 1; \
    int channel_id = lua_tointeger(L, arg_id++); \
    struct GAME_ACTION_STRUCT_NAME(variable) data; \
    DATA_TYPING \
    game_action_set_ ## variable(channel_id, data); \
    return 0; \
}
#define X_FLOAT_DATA(x) data.x = lua_tonumber(L, arg_id++);
#define X_INT_DATA(x) data.x = lua_tointeger(L, arg_id++);
#define X_BOOL_DATA(x) data.x = lua_toboolean(L, arg_id++);

DEFINE_GAME_VARIABLES
#undef X
#undef X_FLOAT_DATA
#undef X_INT_DATA
#undef X_BOOL_DATA


#define X(variable, setter, getter, DATA_TYPING) \
static int game_action_get_ ## variable ## _lua(lua_State *L) \
{ \
    int number_of_args = 0; \
    struct GAME_ACTION_STRUCT_NAME(variable) data; \
    int channel_id = lua_tointeger(L, 1); \
    game_action_get_ ## variable(channel_id, &data); \
    DATA_TYPING \
    return number_of_args; \
}
#define X_FLOAT_DATA(x) \
    lua_pushnumber(L, data.x); \
    number_of_args++;
#define X_INT_DATA(x) \
    lua_pushinteger(L, data.x); \
    number_of_args++;
#define X_BOOL_DATA(x) \
    lua_pushboolean(L, data.x); \
    number_of_args++;

DEFINE_GAME_VARIABLES
#undef X
#undef X_FLOAT_DATA
#undef X_INT_DATA
#undef X_BOOL_DATA

void register_lua_action_functions(lua_State *L)
{
#define X(variable, setter, getter, DATA_TYPING) \
    lua_pushcfunction(L, game_action_set_ ## variable ## _lua); \
    lua_setglobal(L, "set_" # variable); \
    lua_pushcfunction(L, game_action_get_ ## variable ## _lua); \
    lua_setglobal(L, "get_" # variable);
DEFINE_GAME_VARIABLES
#undef X
}

