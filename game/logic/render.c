#include "game/logic/render.h"
#include "game/logic/math.h"
#include "resources.h"
#include "render/primitive.h"

#include <lua.h>
#include <lauxlib.h>
#include <render/base.h>

// metafunctions
static int lua_render_obj_free(lua_State* L)
{
	render_obj* robj = lua_touserdata(L, 1);
	render_obj_free(robj);
	return 0;
}

// API
static int lua_render_obj_draw(lua_State* L)
{
	if(lua_type(L, 1) == LUA_TUSERDATA)
		luaL_checkudata(L, 1, "render_obj");
	else
		luaL_checktype(L, 1, LUA_TLIGHTUSERDATA);
	const render_obj* robj = lua_topointer(L, 1);

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

	glPushMatrix();
	glTranslatef(tr.x, tr.z, tr.y);
	glRotatef(rot.x, 0, 1, 0);
	glRotatef(rot.y, 0, 0, 1);
	glScalef(sc.x, sc.z, sc.y);

	render_obj_draw(robj);
	glPopMatrix();
	return 0;
}

static int lua_render_hexahedron(lua_State* L)
{
	hexahedron h = lua_get_hexahedron(L, 1);
	render_obj* robj_ptr = lua_newuserdata(L, sizeof(render_obj));
	*robj_ptr = render_hexahedron(h);
	luaL_setmetatable(L, "render_obj");
	return 1;
}

static int lua_model_find(lua_State* L)
{
	const char* model_name = luaL_checkstring(L, 1);
	render_obj* robj = model_find(model_name);
	if(robj){
		lua_pushlightuserdata(L, robj);
		return 1;
	}
	return 0;
}

static const struct luaL_Reg cfuncs[] = {
	{"render_obj_draw", lua_render_obj_draw},
	{"render_hexahedron", lua_render_hexahedron},

	{"model_find", lua_model_find},
	{NULL, NULL}
};


void game_logic_init_render(lua_State* _s)
{
	lua_newtable(_s);
	luaL_setfuncs(_s, cfuncs, 0);
	lua_setglobal(_s, "render");

	luaL_newmetatable(_s, "render_obj");
	lua_pushcfunction(_s, lua_render_obj_free); lua_setfield(_s, -2, "__gc");
	lua_pop(_s, 1);
}
