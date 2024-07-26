#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "math/vec.h"
#include "math/collision.h"
#include "dyn_array.h"

typedef struct tnode tnode;
struct tnode {
	int x, z; float y;

	terrain_piece* tpiece;
	float cost, heuristic;
	struct tnode* parent;
};
DEF_DYN_ARRAY(tnode_dynarray, tnode)

typedef struct {
	size_t ln;
	vec2f* points;
	terrain_piece** tpieces;
} path;
#define path_free(p){\
	free((p).points);\
	free((p).tpieces);\
}

#define PATHING_TYPE_EXACT		0
#define PATHING_TYPE_DISTANCE		1
// [args] double distance

path path_find(body* b, vec3f target, int pathing_type, ...);

// bbox_w and bbox_h should be odd. If they aren't, increment by 1.
// tpiece_buf should be an array of bbox_w*bbox_h pointers, initially set to 0.
// returns false on failure to get at least 1 ocuppied tpiece
void get_all_occupied_tpieces(int bbox_w, int bbox_h,
				int cur_x, int cur_z, int buf_x, int buf_z,
				terrain_piece* cur_tpiece,
				terrain_piece** tpiece_buf);
#endif
