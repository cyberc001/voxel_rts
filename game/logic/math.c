#include "game/logic/math.h"

static int lua_vec3_rot(lua_State* L)
{
	vec3f v = lua_get_vec3(L, 1), rot = lua_get_vec3(L, 2);
	lua_push_vec3(L, vec3f_rot(v, rot));
	return 1;
}
static int lua_vec3_lookat_rot(lua_State* L)
{
	vec3f cur_rot = lua_get_vec3(L, 1), look_pt = lua_get_vec3(L, 2);
	lua_push_vec3(L, vec3f_lookat_rot(cur_rot, look_pt));
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
static int lua_hexahedron_transform(lua_State* L)
{
	hexahedron h = lua_get_hexahedron(L, 1);
	vec3f tr = {0, 0, 0};
	vec3f rot = {0, 0, 0};
	vec3f sc = {1, 1, 1};

	for(int i = 2; i <= lua_gettop(L) && i <= 4; ++i){
		if(lua_type(L, i) != LUA_TTABLE)
			break;
		switch(i){
			case 2: tr = lua_get_vec3(L, i); break;
			case 3: rot = lua_get_vec3(L, i); break;
			case 4: sc = lua_get_vec3(L, i); break;
		}
	}

	mat4f trmat = mat4f_identity();
	mat4f_translate(&trmat, tr);
	mat4f_rotate(&trmat, rot.x, (vec3f){1, 0, 0});
	mat4f_rotate(&trmat, rot.y, (vec3f){0, 1, 0});
	mat4f_rotate(&trmat, rot.z, (vec3f){0, 0, 1});
	mat4f_scale(&trmat, sc);
	
	h = hexahedron_transform(&h, &trmat);

	lua_push_hexahedron(L, h);
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
	vec3f resolution; vec3f new_rot = lua_get_vec3(L, 2);
	int collided = hexahedron_check_terrain_collision(&h, &resolution, &new_rot);
	lua_pushboolean(L, collided);
	if(collided){
		lua_push_vec3(L, resolution);
		lua_push_vec3(L, new_rot);
		return 3;
	}
	return 1;
}

static const struct luaL_Reg cfuncs[] = {
	{"vec3_rot", lua_vec3_rot},
	{"vec3_lookat_rot", lua_vec3_lookat_rot},

	{"hexahedron_from_cuboid", lua_hexahedron_from_cuboid},
	{"hexahedron_from_cube", lua_hexahedron_from_cube},
	{"hexahedron_from_cuboid_centered", lua_hexahedron_from_cuboid_centered},
	{"hexahedron_from_cube_centered", lua_hexahedron_from_cube_centered},


	{"hexahedron_transform", lua_hexahedron_transform},
	{"hexahedron_get_center", lua_hexahedron_get_center},

	{"hexahedron_check_collision", lua_hexahedron_check_collision},
	{"hexahedron_check_terrain_collision", lua_hexahedron_check_terrain_collision},
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
