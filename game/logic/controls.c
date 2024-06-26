#include "game/logic/controls.h"
#include "game/logic/math.h"

static int lua_get_selection_query(lua_State* L)
{
	lua_pushinteger(L, controls_selection_queried);
	controls_selection_queried = CONTROLS_SELECTION_NOT_QUERIED;
	return 1;
}
static int lua_get_order_query(lua_State* L)
{
	if(!controls_order_queried)
		return 0;
	lua_push_vec3(L, controls_order_world_coords);
	controls_order_queried = 0;
	return 1;
}

static const struct luaL_Reg cfuncs[] = {
	{"get_selection_query", lua_get_selection_query},
	{"get_order_query", lua_get_order_query},

	{NULL, NULL}
};

void game_logic_init_controls(lua_State* _s)
{
	lua_newtable(_s);
	luaL_setfuncs(_s, cfuncs, 0);
	lua_setglobal(_s, "controls");
}
