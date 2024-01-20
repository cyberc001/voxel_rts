#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "math/vec.h"
#include "math/collision.h"

typedef struct {
	size_t ln;
	vec2f* points;
} path;

path path_find(const hexahedron* h, vec3f target);

#endif
