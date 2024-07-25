#ifndef GAME_BODY_H
#define GAME_BODY_H

#include "game/logic.h"
#include "game/logic/math.h"

#include "math/body.h"
#include "math/hexahedron.h"
#include "math/box.h"

void game_logic_init_body(lua_State* _s);

void* lua_create_body(lua_State* L, int type, size_t size);
void* lua_push_body(lua_State* L, const body* b);

void* lua_get_body(lua_State* L, int index);

body_box* lua_push_body_box(lua_State* L, vec3f _size);
body_hexahedron* lua_push_body_hexahedron(lua_State* L, const hexahedron* h);

#endif
