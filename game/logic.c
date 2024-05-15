#include "game/logic.h"
#include "log.h"
#include "game/logic/render.h"
#include "game/logic/math.h"
#include "ticker.h"

lua_State* global_lua_state;
static const char* base_fname = "logic/main.lua";

static ticker* logic_ticker;
void game_logic_init()
{
	// create Lua state
	global_lua_state = luaL_newstate();
	luaL_openlibs(global_lua_state);

	game_logic_init_render(global_lua_state);
	game_logic_init_math(global_lua_state);
	game_logic_init_path(global_lua_state);
	game_logic_init_controls(global_lua_state);

	if(luaL_loadfile(global_lua_state, base_fname) || lua_pcall(global_lua_state, 0, 0, 0)){
		LOG_ERROR("Error running \"%s\":\n%s\n", base_fname, lua_tostring(global_lua_state, -1));
		lua_pop(global_lua_state, 1);
		return;
	}

	logic_ticker = ticker_create(16, game_logic_tick);
}

static int first_tick = 1;
void game_logic_tick(unsigned ms)
{
	float time_delta = ms / 1000.;

	if(first_tick){
		first_tick = 0;
		lua_getglobal(global_lua_state, "_first_tick");
		if(lua_pcall(global_lua_state, 0, 0, 0)){
			LOG_ERROR("global function _first_tick() returned an error:\n%s\n", lua_tostring(global_lua_state, -1));
			lua_pop(global_lua_state, 1);
		}
	}

	lua_getglobal(global_lua_state, "_tick");
	lua_pushnumber(global_lua_state, time_delta);
	if(lua_pcall(global_lua_state, 1, 0, 0)){
		LOG_ERROR("global function _tick() returned an error:\n%s\n", lua_tostring(global_lua_state, -1));
		lua_pop(global_lua_state, 1);
	}
}
void game_logic_render()
{
	lua_getglobal(global_lua_state, "_render");
	if(lua_pcall(global_lua_state, 0, 0, 0)){
		LOG_ERROR("global function _render() returned an error:\n%s\n", lua_tostring(global_lua_state, -1));
		lua_pop(global_lua_state, 1);
	}
}
