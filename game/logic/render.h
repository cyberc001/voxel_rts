#ifndef GAME_LOGIC_RENDER_H
#define GAME_LOGIC_RENDER_H

#include "game/logic.h"
#include "render/base.h"

void game_logic_init_model(lua_State* _s);

render_obj* get_robj(lua_State* L, int idx);
render_obj* alloc_robj(lua_State* L);

#endif
