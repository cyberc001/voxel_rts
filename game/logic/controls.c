#include "game/logic/controls.h"

static int lua_get_selection_query(lua_State* L)
{
	lua_pushboolean(L, controls_selection_queried);
	controls_selection_queried = 0;
	return 1;
}


static const struct luaL_Reg cfuncs[] = {
	{"get_selection_query", lua_get_selection_query},

	{NULL, NULL}
};

void game_logic_init_controls(lua_State* _s)
{
	lua_newtable(_s);
	luaL_setfuncs(_s, cfuncs, 0);
	lua_setglobal(_s, "controls");
}
