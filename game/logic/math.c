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

static int lua_hexahedron_from_cuboid(lua_State* L)
{
	lua_push_hexahedron(L, hexahedron_from_cuboid(luaL_checknumber(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3)));
	return 1;
}
static int lua_hexahedron_from_cube(lua_State* L)
{
	lua_push_hexahedron(L, hexahedron_from_cube(luaL_checknumber(L, 1)));
	return 1;
}
static int lua_hexahedron_from_cuboid_centered(lua_State* L)
{
	lua_push_hexahedron(L, hexahedron_from_cuboid_centered(luaL_checknumber(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3)));
	return 1;
}
static int lua_hexahedron_from_cube_centered(lua_State* L)
{
	lua_push_hexahedron(L, hexahedron_from_cube_centered(luaL_checknumber(L, 1)));
	return 1;
}

static int lua_hexahedron_get_center(lua_State* L)
{
	hexahedron h = lua_get_hexahedron(L, 1);
	lua_push_vec3(L, hexahedron_get_center(&h));
	return 1;
}
static int lua_hexahedron_get_bbox(lua_State* L)
{
	hexahedron h = lua_get_hexahedron(L, 1);
	lua_push_bbox(L, hexahedron_get_bbox(&h));
	return 1;
}
static int lua_hexahedron_get_interaction_box(lua_State* L)
{
	hexahedron h = lua_get_hexahedron(L, 1);
	float expand = lua_isnumber(L, 2) ? lua_tonumber(L, 2) : 0.2;
	lua_push_bbox(L, hexahedron_get_interaction_box(&h, expand));
	return 1;
}

static int lua_hexahedron_transform(lua_State* L)
{
	hexahedron h = lua_get_hexahedron(L, 1);
	vec3f tr = {0, 0, 0};
	vec4f rot = {0, 0, 0, 1};
	vec3f sc = {1, 1, 1};

	if(lua_type(L, 2) == LUA_TTABLE)
		tr = lua_get_vec3(L, 2);
	if(lua_type(L, 3) == LUA_TTABLE)
		rot = lua_get_vec4(L, 3);
	if(lua_type(L, 4) == LUA_TTABLE)
		sc = lua_get_vec3(L, 4);

	mat4f rot_mat = mat_from_quat(rot);
	for(size_t i = 0; i < 6; ++i)
		for(size_t j = 0; j < 4; ++j){
			vec4f v = vec4f_mul_mat4f(&rot_mat, (vec4f){h.f[i].p[j].x,
					h.f[i].p[j].y, h.f[i].p[j].z, 1});
			h.f[i].p[j] = (vec3f){v.x, v.y, v.z};
		}

	mat4f trmat = mat4f_identity();
	mat4f_translate(&trmat, tr);
	mat4f_scale(&trmat, sc);
		
	h = hexahedron_transform(&h, &trmat);

	lua_push_hexahedron(L, h);
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

static int lua_hexahedron_check_collision(lua_State* L)
{
	hexahedron h1 = lua_get_hexahedron(L, 1);
	hexahedron h2 = lua_get_hexahedron(L, 2);
	vec3f resolution;
	int collided = hexahedron_check_collision(&h1, &h2, &resolution);
	lua_pushboolean(L, collided);
	if(collided){
		lua_push_vec3(L, resolution);
		return 2;
	}
	return 1;
}
static int lua_hexahedron_check_terrain_collision(lua_State* L)
{
	hexahedron h = lua_get_hexahedron(L, 1);
	vec3f resolution; 
	vec3f forward = (vec3f){1, 0, 0};
	if(lua_istable(L, 2))
		forward = lua_get_vec3(L, 2);
	vec4f new_rot = {0, 0, 0, 1};

	int collided = hexahedron_check_terrain_collision(&h, &resolution, forward, &new_rot);
	lua_pushboolean(L, collided);
	if(collided){
		lua_push_vec3(L, resolution);
		lua_push_vec4(L, new_rot);
		return 3;
	}
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

#define SET_MIN(where, what) {if((what) < (where)) (where) = (what);}
#define SET_MAX(where, what) {if((what) > (where)) (where) = (what);}
static int lua_hexahedron_is_selected(lua_State* L)
{
	hexahedron h = lua_get_hexahedron(L, 1);
	bbox3f bbox = hexahedron_get_bbox(&h);
	vec2f proj_min = {INFINITY, INFINITY}, proj_max = {-INFINITY, -INFINITY};
	for(int max_bits = 0; max_bits < 8; ++max_bits){
		vec3f pt = {
			max_bits & 1 ? bbox.max.x : bbox.min.x,
			max_bits & 2 ? bbox.max.y : bbox.min.y,
			max_bits & 4 ? bbox.max.z : bbox.min.z,
		};
		vec2f proj = vec3f_project2(pt);
		SET_MIN(proj_min.x, proj.x);
		SET_MIN(proj_min.y, proj.y);
		SET_MAX(proj_max.x, proj.x);
		SET_MAX(proj_max.y, proj.y);
	}

	lua_pushboolean(L,
			proj_min.x < controls_selection_max.x && proj_max.x > controls_selection_min.x &&
			proj_min.y < controls_selection_max.y && proj_max.y > controls_selection_min.y);
	return 1;
}

static const struct luaL_Reg cfuncs[] = {
	{"vec3_quat_rot", lua_vec3_quat_rot},

	{"hexahedron_from_cuboid", lua_hexahedron_from_cuboid},
	{"hexahedron_from_cube", lua_hexahedron_from_cube},
	{"hexahedron_from_cuboid_centered", lua_hexahedron_from_cuboid_centered},
	{"hexahedron_from_cube_centered", lua_hexahedron_from_cube_centered},

	{"hexahedron_get_center", lua_hexahedron_get_center},
	{"hexahedron_get_bbox", lua_hexahedron_get_bbox},
	{"hexahedron_get_interaction_box", lua_hexahedron_get_interaction_box},

	{"hexahedron_transform", lua_hexahedron_transform},

	{"interp_quat", lua_interp_quat},

	{"hexahedron_check_collision", lua_hexahedron_check_collision},
	{"hexahedron_check_terrain_collision", lua_hexahedron_check_terrain_collision},
	{"bbox_check_collision", lua_bbox_check_collision},
	{"bbox_contains_bbox", lua_bbox_contains_bbox},

	{"hexahedron_is_selected", lua_hexahedron_is_selected},

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
}
void lua_push_vec3(lua_State* L, vec3f v)
{
	lua_newtable(L);
	lua_pushstring(L, "x"); lua_pushnumber(L, v.x); lua_settable(L, -3);
	lua_pushstring(L, "y"); lua_pushnumber(L, v.y); lua_settable(L, -3);
	lua_pushstring(L, "z"); lua_pushnumber(L, v.z); lua_settable(L, -3);
}
void lua_push_vec4(lua_State* L, vec4f v)
{
	lua_newtable(L);
	lua_pushstring(L, "x"); lua_pushnumber(L, v.x); lua_settable(L, -3);
	lua_pushstring(L, "y"); lua_pushnumber(L, v.y); lua_settable(L, -3);
	lua_pushstring(L, "z"); lua_pushnumber(L, v.z); lua_settable(L, -3);
	lua_pushstring(L, "w"); lua_pushnumber(L, v.w); lua_settable(L, -3);
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

hexahedron lua_get_hexahedron(lua_State* L, int hexaidx)
{
	hexahedron h;
	for(size_t i = 0; i < 6; ++i){ // for each face
		lua_rawgeti(L, hexaidx, i+1);
		for(size_t j = 0; j < 4; ++j){ // for each vertex
			lua_rawgeti(L, -1, j+1);
			h.f[i].p[j] = lua_get_vec3(L, -1);
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
	}
	return h;
}
void lua_push_hexahedron(lua_State* L, hexahedron h)
{
	lua_newtable(L);
	for(size_t i = 0; i < 6; ++i){ // for each face
		lua_newtable(L);
		for(size_t j = 0; j < 4; ++j){ // for each vertex
			lua_push_vec3(L, h.f[i].p[j]);
			lua_rawseti(L, -2, j+1);
		}
		lua_rawseti(L, -2, i+1);
	}
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
