#include "game/logic.h"
#include "log.h"
#include "game/logic/render.h"
#include "game/logic/math.h"

static lua_State* _s;
static const char* base_fname = "logic/main.lua";

void game_logic_init()
{
	// create Lua state
	_s = luaL_newstate();
	luaL_openlibs(_s);

	game_logic_init_render(_s);
	game_logic_init_math(_s);

	if(luaL_loadfile(_s, base_fname) || lua_pcall(_s, 0, 0, 0)){
		LOG_ERROR("Error running \"%s\":\n%s\n", base_fname, lua_tostring(_s, -1));
		lua_pop(_s, 1);
		return;
	}
}

void game_logic_tick()
{
	lua_getglobal(_s, "_tick");
	if(lua_pcall(_s, 0, 0, 0)){
		LOG_ERROR("global logic _tick() returned an error:\n%s\n", lua_tostring(_s, -1));
		lua_pop(_s, 1);
	}
	lua_pop(_s, 1);
}
void game_logic_render()
{
	lua_getglobal(_s, "_render");
	if(lua_pcall(_s, 0, 0, 0)){
		LOG_ERROR("global logic _render() returned an error:\n%s\n", lua_tostring(_s, -1));
		lua_pop(_s, 1);
	}
}
