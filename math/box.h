#ifndef MATH_BOX_H
#define MATH_BOX_H

#include "math/vec.h"
#include "math/body.h"

/* body */

typedef struct {
	struct body;
	vec3f _size; // the box is centered at (_size.x/2, _size.y/2, _size.z/2)
	vec3f side_cache[3]; // each side's length is halved
} body_box;

vec3f box_get_vertice(vec3f _size, size_t idx);
vec3f box_get_cached_vertice(vec3f* side_cache, vec3f pos, size_t idx);
axes_list box_get_separating_axes_and_edges(vec3f* side_cache, size_t* normals_cnt);

#endif
