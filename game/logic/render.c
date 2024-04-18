#include "game/logic/render.h"
#include "game/logic/math.h"
#include "resources.h"
#include "render/primitive.h"
#include "math/quat.h"

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
	vec4f rot = {0, 0, 0, 1};
	vec3f sc = {1, 1, 1};

	if(lua_type(L, 2) == LUA_TTABLE)
		tr = lua_get_vec3(L, 2);
	if(lua_type(L, 3) == LUA_TTABLE)
		rot = lua_get_vec4(L, 3);
	if(lua_type(L, 4) == LUA_TTABLE)
		sc = lua_get_vec3(L, 4);

	glPushMatrix();
	glTranslatef(tr.x, tr.y, tr.z);
	mat4f rot_mat = mat_from_quat(rot);
	GLfloat trmatf[] = { rot_mat.e[0][0], rot_mat.e[0][1], rot_mat.e[0][2], rot_mat.e[0][3],
		rot_mat.e[1][0], rot_mat.e[1][1], rot_mat.e[1][2], rot_mat.e[1][3],
		rot_mat.e[2][0], rot_mat.e[2][1], rot_mat.e[2][2], rot_mat.e[2][3],
		rot_mat.e[3][0], rot_mat.e[3][1], rot_mat.e[3][2], rot_mat.e[3][3],
	};
	glScalef(sc.x, sc.y, sc.z);
	glMultMatrixf(trmatf);

	render_obj_draw(robj);
	glPopMatrix();
	return 0;
}

static int lua_render_hexahedron(lua_State* L)
{
	hexahedron h = lua_get_hexahedron(L, 1);
	render_obj* robj_ptr = lua_newuserdata(L, sizeof(render_obj));
	*robj_ptr = render_hexahedron(h);
	if(!lua_isnoneornil(L, 2)){
		luaL_checktype(L, 2, LUA_TTABLE);
		vec3f clr = lua_get_vec3(L, 2);
		robj_ptr->colorize = clr;
	}
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
