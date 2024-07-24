#include "game/logic/controls.h"
#include "game/logic/math.h"

static int lua_get_selection_query(lua_State* L)
{
	int cur_query = CONTROLS_SELQUERY_NONE;
	asm volatile("xchg %0, %1"
			: "+r"(cur_query), "+rm"(controls_selection_queried));

	lua_pushinteger(L, cur_query);
	return 1;
}
static int lua_get_order_pos(lua_State* L)
{
	lua_push_vec3(L, controls_order_world_coords);
	return 1;
}

static const struct luaL_Reg cfuncs[] = {
	{"get_selection_query", lua_get_selection_query},
	{"get_order_pos", lua_get_order_pos},

	{NULL, NULL}
};

void game_logic_init_controls(lua_State* _s)
{
	lua_newtable(_s);
	luaL_setfuncs(_s, cfuncs, 0);
	lua_setglobal(_s, "controls");
}
