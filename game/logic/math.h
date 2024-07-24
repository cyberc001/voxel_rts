#ifndef GAME_MATH_H
#define GAME_MATH_H

#include "game/logic.h"
#include "math/collision.h"

void game_logic_init_math(lua_State* _s);

/* Helper functions */

vec2f lua_get_vec2(lua_State* L, int vecidx);
vec3f lua_get_vec3(lua_State* L, int vecidx);
vec4f lua_get_vec4(lua_State* L, int vecidx);
void lua_push_vec2(lua_State* L, vec2f v);
void lua_push_vec3(lua_State* L, vec3f v);
void lua_push_vec4(lua_State* L, vec4f v);

void lua_push_mat4(lua_State* L, mat4f* m);
mat4f lua_get_mat4(lua_State* L, int matidx);

bbox3f lua_get_bbox(lua_State* L, int bboxidx);
void lua_push_bbox(lua_State* L, bbox3f bbox);

#endif
