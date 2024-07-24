#include "game/logic.h"
#include "log.h"
#include "game/logic/render.h"
#include "game/logic/math.h"
#include "game/logic/body.h"
#include "ticker.h"

/* Initialization and state */

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
	game_logic_init_body(global_lua_state);
	game_logic_init_controls(global_lua_state);

	if(luaL_loadfile(global_lua_state, base_fname) || lua_pcall(global_lua_state, 0, 0, 0)){
		LOG_ERROR("Error running \"%s\":\n%s\n", base_fname, lua_tostring(global_lua_state, -1));
		lua_pop(global_lua_state, 1);
		return;
	}

	logic_ticker = ticker_create(16, game_logic_tick);
}

/* Error handling */

static int game_logic_error(lua_State* L)
{
	const char* msg = luaL_checkstring(L, 1);
	LOG_ERROR("%s", msg);

	LOG_NOPREFIX("*** Stack trace: ***");
	int lvl = 0;
	lua_Debug ar;
	while(lua_getstack(L, ++lvl, &ar)){
		lua_getinfo(L, "Snl", &ar);
		if(ar.name)
			LOG_NOPREFIX("%d. %s()", lvl, ar.name);
		else
			LOG_NOPREFIX("%d. in-line", lvl);
		LOG_NOPREFIX("\t%s:%d", ar.short_src, ar.currentline);
	}
	LOG_NOPREFIX("");
	return 0;
}

/* Tick functions */

static int first_tick = 1;
void game_logic_tick(unsigned ms)
{
	float time_delta = ms / 1000.;

	if(first_tick){
		first_tick = 0;
		lua_pushcfunction(global_lua_state, game_logic_error);
		lua_getglobal(global_lua_state, "_first_tick");
		lua_pcall(global_lua_state, 0, 0, -2);
	}

	lua_pushcfunction(global_lua_state, game_logic_error);
	lua_getglobal(global_lua_state, "_tick");
	lua_pushnumber(global_lua_state, time_delta);
	lua_pcall(global_lua_state, 1, 0, -3);
}
void game_logic_render()
{
	lua_getglobal(global_lua_state, "_render");
	lua_pcall(global_lua_state, 0, 0, 0);
}
