#include "game/logic/math.h"

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
static int lua_hexahedron_transform(lua_State* L)
{
	hexahedron h = lua_get_hexahedron(L, 1);
	vec3f tr = {0, 0, 0};
	vec2f rot = {0, 0};
	vec3f sc = {1, 1, 1};

	for(int i = 2; i <= lua_gettop(L) && i <= 4; ++i){
		if(lua_type(L, i) != LUA_TTABLE)
			break;
		switch(i){
			case 2: tr = lua_get_vec3(L, i); break;
			case 3: rot = lua_get_vec2(L, i); break;
			case 4: sc = lua_get_vec3(L, i); break;
		}
	}

	mat4f trmat = mat4f_identity();
	mat4f_translate(&trmat, (vec3f){tr.x, /*tr.z*/0, /*tr.y*/0});
	//mat4f_rotate(&trmat, rot.x, (vec3f){0, 1, 0});
	//mat4f_rotate(&trmat, rot.y, (vec3f){0, 0, 1});
	mat4f_scale(&trmat, (vec3f){sc.x, /*sc.z*/1, /*sc.y*/1});
	
	h = hexahedron_transform(&h, &trmat);

	lua_push_hexahedron(L, h);
	return 1;

	/*glTranslatef(tr.x, tr.z, tr.y);
	glRotatef(rot.x, 0, 1, 0);
	glRotatef(rot.y, 0, 0, 1);
	glScalef(sc.x, sc.z, sc.y);*/
}
static int lua_hexahedron_check_collision(lua_State* L)
{
	hexahedron h1 = lua_get_hexahedron(L, 1);
	hexahedron h2 = lua_get_hexahedron(L, 2);

	lua_pushinteger(L, hexahedron_check_collision(&h1, &h2));
	return 1;
}

static const struct luaL_Reg cfuncs[] = {
	{"hexahedron_from_cuboid", lua_hexahedron_from_cuboid},
	{"hexahedron_from_cube", lua_hexahedron_from_cube},
	{"hexahedron_transform", lua_hexahedron_transform},
	{"hexahedron_check_collision", lua_hexahedron_check_collision},
	{NULL, NULL}
};

void game_logic_init_math(lua_State* _s)
{
	lua_newtable(_s);
	luaL_setfuncs(_s, cfuncs, 0);
	lua_setglobal(_s, "math");
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
