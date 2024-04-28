#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "more_math.h"
#include <lua.h>
#include <lauxlib.h>

extern lua_State* global_lua_state;
void game_logic_init();

void game_logic_tick();
void game_logic_render();

#define lua_to_const_idx(L, i) ((i) > 0 ? (i) : (lua_gettop(L) + 1 + (i)))

#endif
