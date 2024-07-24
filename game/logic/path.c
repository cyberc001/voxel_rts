#include "game/logic/path.h"
#include "math/collision.h"
#include "game/pathfinding.h"
#include "game/terrain.h"
#include "game/logic/body.h"
#include "log.h"

static int lua_find_path(lua_State* L)
{
	body* b = lua_get_body(L, 1);
	vec3f target = lua_get_vec3(L, 2);

	path out_p;
	if(lua_isnumber(L, 3)){
		vec2f pitch_range = lua_istable(L, 4) ? lua_get_vec2(L, 4) : (vec2f){-90, 90};
		out_p = path_find(b, target, PATHING_TYPE_DISTANCE, lua_tonumber(L, 4), pitch_range);
	}
	else
		out_p = path_find(b, target, PATHING_TYPE_EXACT);

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

int check_bbox_octree_collision(lua_State* L, bbox3f bbox)
{
	lua_getglobal(L, "_check_bbox_octree_collision");
	lua_push_bbox(L, bbox);
	if(lua_pcall(L, 1, 1, 0)){
		LOG_ERROR("function _check_bbox_octree_collision returned an error:\n%s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return 0;
	}
	return lua_toboolean(L, -1);
}
