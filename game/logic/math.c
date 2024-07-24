#include "game/logic/math.h"
#include "controls/selection.h"
#include "math/quat.h"

static int lua_vec3_quat_rot(lua_State* L)
{
	vec3f v = lua_get_vec3(L, 1);
	vec4f rot = lua_get_vec4(L, 2);
	mat4f rot_mat = mat_from_quat(rot);
	vec4f _out = vec4f_mul_mat4f(&rot_mat, (vec4f){v.x, v.y, v.z, 1});
	lua_push_vec3(L, (vec3f){_out.x, _out.y, _out.z});
	return 1;
}
static int lua_quat_rot_from_vec3(lua_State* L)
{
	vec3f dir = lua_get_vec3(L, 1);
	lua_push_vec4(L, quat_from_rot_between(dir, (vec3f){1, 0, 0}));
	return 1;
}

static int lua_quat_from_rot(lua_State* L)
{
	vec3f rot = lua_get_vec3(L, 1);
	lua_push_vec4(L, quat_from_rot(rot));
	return 1;
}
static int lua_rot_from_quat(lua_State* L)
{
	vec4f quat = lua_get_vec4(L, 1);
	lua_push_vec3(L, rot_from_quat(quat));
	return 1;
}

static int lua_axis_from_quat(lua_State* L)
{
	vec4f quat = lua_get_vec4(L, 1);
	lua_push_vec3(L, axis_from_quat(quat));
	return 1;
}
static int lua_quat_from_axis(lua_State* L)
{
	vec3f axis = lua_get_vec3(L, 1);
	lua_push_vec4(L, quat_from_axis(axis));
	return 1;
}

static int lua_interp_quat(lua_State* L)
{
	vec4f quat_cur = vec4_norm(lua_get_vec4(L, 1));
	vec4f quat_end = vec4_norm(lua_get_vec4(L, 2));
	float frac = luaL_checknumber(L, 3);

	vec4f quat_m = quat_slerp(quat_cur, quat_end, frac);
	lua_push_vec4(L, quat_m);
	return 1;
}

static int lua_bbox_check_collision(lua_State* L)
{
	bbox3f b1 = lua_get_bbox(L, 1);
	bbox3f b2 = lua_get_bbox(L, 2);
	lua_pushboolean(L, bbox_check_collision(&b1, &b2));
	return 1;
}
static int lua_bbox_contains_bbox(lua_State* L)
{
	bbox3f container = lua_get_bbox(L, 1);
	bbox3f containee = lua_get_bbox(L, 2);
	lua_pushboolean(L, bbox_contains_bbox(&container, &containee));
	return 1;
}

static const struct luaL_Reg cfuncs[] = {
	{"vec3_quat_rot", lua_vec3_quat_rot},
	{"quat_rot_from_vec3", lua_quat_rot_from_vec3},
	{"quat_from_rot", lua_quat_from_rot},
	{"rot_from_quat", lua_rot_from_quat},
	{"axis_from_quat", lua_axis_from_quat},
	{"quat_from_axis", lua_quat_from_axis},

	{"interp_quat", lua_interp_quat},

	{"bbox_check_collision", lua_bbox_check_collision},
	{"bbox_contains_bbox", lua_bbox_contains_bbox},

	{NULL, NULL}
};

void game_logic_init_math(lua_State* _s)
{
	lua_newtable(_s);
	luaL_setfuncs(_s, cfuncs, 0);
	lua_setglobal(_s, "gmath");
}

/* Helper functions */
vec2f lua_get_vec2(lua_State* L, int vecidx)
{
	vecidx = lua_to_const_idx(L, vecidx);
	lua_pushstring(L, "y");
	lua_gettable(L, vecidx);
	lua_pushstring(L, "x");
	lua_gettable(L, vecidx);
	vec2f v = {lua_tonumber(L, -1), lua_tonumber(L, -2)};
	lua_pop(L, 2);
	return v;
}
vec3f lua_get_vec3(lua_State* L, int vecidx)
{
	vecidx = lua_to_const_idx(L, vecidx);
	lua_pushstring(L, "z"); lua_gettable(L, vecidx);
	lua_pushstring(L, "y"); lua_gettable(L, vecidx);
	lua_pushstring(L, "x"); lua_gettable(L, vecidx);
	vec3f v = {lua_tonumber(L, -1), lua_tonumber(L, -2), lua_tonumber(L, -3)};
	lua_pop(L, 3);
	return v;
}
vec4f lua_get_vec4(lua_State* L, int vecidx)
{
	vecidx = lua_to_const_idx(L, vecidx);
	lua_pushstring(L, "w"); lua_gettable(L, vecidx);
	lua_pushstring(L, "z"); lua_gettable(L, vecidx);
	lua_pushstring(L, "y"); lua_gettable(L, vecidx);
	lua_pushstring(L, "x"); lua_gettable(L, vecidx);
	vec4f v = {lua_tonumber(L, -1), lua_tonumber(L, -2), lua_tonumber(L, -3), lua_tonumber(L, -4)};
	lua_pop(L, 4);
	return v;
}
void lua_push_vec2(lua_State* L, vec2f v)
{
	lua_newtable(L);
	lua_pushstring(L, "x"); lua_pushnumber(L, v.x); lua_settable(L, -3);
	lua_pushstring(L, "y"); lua_pushnumber(L, v.y); lua_settable(L, -3);

	lua_getglobal(L, "vec2");
	lua_setmetatable(L, -2);
}
void lua_push_vec3(lua_State* L, vec3f v)
{
	lua_newtable(L);
	lua_pushstring(L, "x"); lua_pushnumber(L, v.x); lua_settable(L, -3);
	lua_pushstring(L, "y"); lua_pushnumber(L, v.y); lua_settable(L, -3);
	lua_pushstring(L, "z"); lua_pushnumber(L, v.z); lua_settable(L, -3);

	lua_getglobal(L, "vec3");
	lua_setmetatable(L, -2);
}
void lua_push_vec4(lua_State* L, vec4f v)
{
	lua_newtable(L);
	lua_pushstring(L, "x"); lua_pushnumber(L, v.x); lua_settable(L, -3);
	lua_pushstring(L, "y"); lua_pushnumber(L, v.y); lua_settable(L, -3);
	lua_pushstring(L, "z"); lua_pushnumber(L, v.z); lua_settable(L, -3);
	lua_pushstring(L, "w"); lua_pushnumber(L, v.w); lua_settable(L, -3);

	lua_getglobal(L, "vec4");
	lua_setmetatable(L, -2);
}

void lua_push_mat4(lua_State* L, mat4f* m)
{
	lua_newtable(L);
	for(size_t i = 0; i < 4; ++i)
		for(size_t j = 0; j < 4; ++j){
			lua_pushinteger(L, i*4 + j + 1);
			lua_pushnumber(L, m->e[i][j]);
			lua_settable(L, -3);
		}
}
mat4f lua_get_mat4(lua_State* L, int matidx)
{
	matidx = lua_to_const_idx(L, matidx);
	mat4f m;
	for(size_t i = 0; i < 16; ++i){
		lua_pushinteger(L, i+1);
		lua_gettable(L, matidx);
		m.e[i / 4][i % 4] = lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	return m;
}

bbox3f lua_get_bbox(lua_State* L, int bboxidx)
{
	bbox3f bbox;
	lua_rawgeti(L, bboxidx, 1);
	bbox.min = lua_get_vec3(L, -1);
	lua_pop(L, 1);
	lua_rawgeti(L, bboxidx, 2);
	bbox.max = lua_get_vec3(L, -1);
	lua_pop(L, 1);
	return bbox;
}
void lua_push_bbox(lua_State* L, bbox3f bbox)
{
	lua_newtable(L);
	lua_push_vec3(L, bbox.min);
	lua_rawseti(L, -2, 1);
	lua_push_vec3(L, bbox.max);
	lua_rawseti(L, -2, 2);
}
