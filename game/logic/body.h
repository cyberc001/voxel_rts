#ifndef GAME_BODY_H
#define GAME_BODY_H

#include "game/logic.h"
#include "game/logic/math.h"
#include "math/body.h"
#include "math/hexahedron.h"

void game_logic_init_body(lua_State* _s);

void* lua_create_body(lua_State* L, int type, size_t size);
void* lua_push_body(lua_State* L, const body* b);

void* lua_get_body(lua_State* L, int index);

body_hexahedron* lua_push_hexahedron(lua_State* L, const hexahedron* h);

#endif
