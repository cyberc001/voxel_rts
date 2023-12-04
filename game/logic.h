#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "more_math.h"
#include <lua.h>
#include <lauxlib.h>

void game_logic_init();

void game_logic_tick();
void game_logic_render();

/* Helper functions */
vec2f lua_get_vec2(lua_State* L, int vecidx);
vec3f lua_get_vec3(lua_State* L, int vecidx);

#endif
