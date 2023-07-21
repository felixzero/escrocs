#pragma once

#include <stdbool.h>

#include "lua/lua.h"

void init_lua_action_functions(void);

void register_lua_action_functions(lua_State *L);

void set_lua_action_function_enable_status(bool enabled);
