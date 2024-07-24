#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "math/vec.h"
#include "math/collision.h"
#include "dyn_array.h"

typedef struct tnode tnode;
struct tnode {
	vec2i pos; float y;
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

path path_find(body* b, vec3f target,
		int pathing_type, ...);

// bbox_w and bbox_h should be odd. If they aren't, increment by 1.
// tpiece_buf should be an array of bbox_w*bbox_h pointers, initially set to 0.
void get_all_occupied_tpieces(int bbox_w, int bbox_h,
				vec2i cur_pos, terrain_piece* cur_tpiece,
				terrain_piece** tpiece_buf, tnode_dynarray* node_buf);

#endif
