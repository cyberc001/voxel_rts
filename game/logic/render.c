#include "game/logic/render.h"
#include "game/logic/math.h"
#include "resources.h"
#include "render/list.h"
#include "render/primitive.h"

#include <lua.h>
#include <lauxlib.h>
#include <render/base.h>

static render_obj* get_robj(lua_State* L, int idx)
{
	if(lua_type(L, idx) == LUA_TUSERDATA)
		luaL_checkudata(L, idx, "render_obj");
	else
		luaL_checktype(L, idx, LUA_TLIGHTUSERDATA);

	render_obj* robj = lua_touserdata(L, idx);
	if(lua_type(L, 1) == LUA_TUSERDATA) // "unpack" userdata that stores pointer to render_obj
		robj = *((render_obj**)robj);

	return robj;
}
static render_obj* alloc_robj(lua_State* L)
{
	render_obj** udata_ptr = lua_newuserdata(L, sizeof(render_obj*));
	render_obj* robj = malloc(sizeof(render_obj));
	*udata_ptr = robj;
	luaL_setmetatable(L, "render_obj");
	return robj;
}

// metafunctions
static int lua_render_obj_free(lua_State* L)
{
	render_obj** robj_ptr = lua_touserdata(L, 1);
	if((*robj_ptr)->flags & RENDER_OBJ_FLAG_SHALLOW_COPY){
		free(*robj_ptr);
	} else {
		render_info_free inf = {*robj_ptr};
		render_list_add_free(&inf);
	}
	return 0;
}

// API
static int lua_render_obj_shallow_copy(lua_State* L)
{
	render_obj* robj = get_robj(L, 1);
	render_obj* new_robj = alloc_robj(L);
	*new_robj = *robj;
	new_robj->flags |= RENDER_OBJ_FLAG_SHALLOW_COPY;
	return 1;
}
static int lua_render_obj_set_cut(lua_State* L)
{
	render_obj* robj = get_robj(L, 1);
	robj->cut_min = lua_get_vec3(L, 2);
	robj->cut_max = lua_get_vec3(L, 3);
	return 0;
}

static int lua_render_obj_draw(lua_State* L)
{
	render_obj* robj = get_robj(L, 1);

	vec3f tr = {0, 0, 0};
	vec4f rot = {0, 0, 0, 1};
	vec3f sc = {1, 1, 1};

	if(lua_type(L, 2) == LUA_TTABLE)
		tr = lua_get_vec3(L, 2);
	if(lua_type(L, 3) == LUA_TTABLE)
		rot = lua_get_vec4(L, 3);
	if(lua_type(L, 4) == LUA_TTABLE)
		sc = lua_get_vec3(L, 4);

	render_info_draw inf = {robj, tr, rot, sc};
	render_list_add_draw(&inf);
	return 0;
}

static int lua_swap_buffers(lua_State* L)
{
	render_list_swap_back_and_mid();
	return 0;
}

static int lua_model_find(lua_State* L)
{
	const char* model_name = luaL_checkstring(L, 1);
	voxel_model* model = model_find(model_name);
	if(model){
		lua_pushlightuserdata(L, &model->model);
		return 1;
	}
	return 0;
}
static int lua_model_data_find(lua_State* L)
{
	const char* model_name = luaL_checkstring(L, 1);
	voxel_model* model = model_find(model_name);
	if(model){
		lua_createtable(L, 2, 0);
		lua_push_vec3(L, model->origin);
		lua_setfield(L, -2, "origin");
		lua_push_vec3(L, model->size);
		lua_setfield(L, -2, "size");
		return 1;
	}
	return 0;
}

static const struct luaL_Reg cfuncs[] = {
	{"render_obj_shallow_copy", lua_render_obj_shallow_copy},
	{"render_obj_set_cut", lua_render_obj_set_cut},

	{"render_obj_draw", lua_render_obj_draw},

	{"swap_buffers", lua_swap_buffers},

	{"model_find", lua_model_find},
	{"model_data_find", lua_model_data_find},

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
