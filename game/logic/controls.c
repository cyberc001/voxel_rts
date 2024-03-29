#include "game/logic/controls.h"

static int lua_get_selection_query(lua_State* L)
{
	if(!controls_selection_queried)
		return 0;

	vec2f vmin = {min(controls_selection_start.x, controls_selection_end.x),
			min(controls_selection_start.y, controls_selection_end.y)};
	vec2f vmax = {max(controls_selection_start.x, controls_selection_end.x),
			max(controls_selection_start.y, controls_selection_end.y)};
	lua_push_vec2(L, vmin);
	lua_push_vec2(L, vmax);
	controls_selection_queried = 0;
	return 2;
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
