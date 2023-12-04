#include "game/logic.h"
#include "log.h"

#include "resources.h"

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
static int lua_render_obj_draw(lua_State* L)
{
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

static lua_State* _s;
static const char* base_fname = "logic/main.lua";
static const struct luaL_Reg cfuncs[] = {
	{"model_find", lua_model_find},
	{"render_obj_draw", lua_render_obj_draw},
	{NULL, NULL}
};

void game_logic_init()
{
	// create Lua state
	_s = luaL_newstate();
	luaL_openlibs(_s);

	lua_newtable(_s);
	luaL_setfuncs(_s, cfuncs, 0);
	lua_setglobal(_s, "game");

	if(luaL_loadfile(_s, base_fname) || lua_pcall(_s, 0, 0, 0)){
		LOG_ERROR("Error running \"%s\":\n%s\n", base_fname, lua_tostring(_s, -1));
		lua_pop(_s, 1);
		return;
	}
}

void game_logic_tick()
{
	lua_getglobal(_s, "tick");
	if(lua_pcall(_s, 0, 0, 0)){
		LOG_ERROR("global logic tick() returned an error:\n%s\n", lua_tostring(_s, -1));
		lua_pop(_s, 1);
	}
	lua_pop(_s, 1);
}
void game_logic_render()
{
	lua_getglobal(_s, "render");
	if(lua_pcall(_s, 0, 0, 0)){
		LOG_ERROR("global logic render() returned an error:\n%s\n", lua_tostring(_s, -1));
		lua_pop(_s, 1);
	}
}

/* Helper functions */
vec2f lua_get_vec2(lua_State* L, int vecidx)
{
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
	lua_pushstring(L, "z");
	lua_gettable(L, vecidx);
	lua_pushstring(L, "y");
	lua_gettable(L, vecidx);
	lua_pushstring(L, "x");
	lua_gettable(L, vecidx);
	vec3f v = {lua_tonumber(L, -1), lua_tonumber(L, -2), lua_tonumber(L, -3)};
	lua_pop(L, 3);
	return v;
}
