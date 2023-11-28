#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <lua.h>
#include <lauxlib.h>

void game_logic_init();

void game_logic_tick();
void game_logic_render();

#endif
