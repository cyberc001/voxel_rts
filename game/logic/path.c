#include "game/logic/path.h"
#include "math/collision.h"
#include "game/pathfinding.h"
#include "game/terrain.h"

static int lua_find_path(lua_State* L)
{
	hexahedron h = lua_get_hexahedron(L, 1);
	vec3f pos = lua_get_vec3(L, 2);
	swap(pos.y, pos.z);
	vec3f target = lua_get_vec3(L, 3);
	path out_p = path_find(&h, pos, target);
	// push path node positions
	lua_createtable(L, out_p.ln, 0);
	for(size_t i = 0; i < out_p.ln; ++i){
		lua_pushinteger(L, i+1);
		lua_push_vec2(L, out_p.points[i]);
		lua_settable(L, -3);
	}
	// push terrain pieces
	lua_createtable(L, out_p.ln, 0);
	for(size_t i = 0; i < out_p.ln; ++i){
		lua_pushinteger(L, i+1);
		lua_pushlightuserdata(L, out_p.tpieces[i]);
		lua_settable(L, -3);
	}
	path_free(out_p);
	return 2;
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
