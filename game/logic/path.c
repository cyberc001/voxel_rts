#include "game/logic/path.h"
#include "math/collision.h"
#include "game/pathfinding.h"

static int lua_find_path(lua_State* L)
{
	hexahedron h = lua_get_hexahedron(L, 1);
	vec3f target = lua_get_vec3(L, 2);
	path_find(&h, target);
	return 0;
}

static const struct luaL_Reg cfuncs[] = {
	{"find_path", lua_find_path},
	{NULL, NULL}
};

void game_logic_init_path(lua_State* _s)
{
	lua_newtable(_s);
	luaL_setfuncs(_s, cfuncs, 0);
	lua_setglobal(_s, "path");
}
