#ifndef PATH_H
#define PATH_H

#include "game/logic.h"
#include "game/logic/math.h"

void game_logic_init_path(lua_State* _s);

int check_bbox_octree_collision(lua_State* L, bbox3f bbox);

#endif
