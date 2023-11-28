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

	float tparams[8] = { // transformation params
		0, 0, 0, // translation
		0, 0, // rotation
		1, 1, 1 // scale
	};

	for(int i = 2; i <= lua_gettop(L); ++i)
		tparams[i - 2] = luaL_checknumber(L, i);

	glPushMatrix();
	glTranslatef(tparams[0], tparams[2], tparams[1]);
	glRotatef(tparams[3], 0, 1, 0);
	glRotatef(tparams[4], 0, 0, 1);
	glScalef(tparams[5], tparams[6], tparams[7]);
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
