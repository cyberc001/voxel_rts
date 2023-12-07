#ifndef MATH_COLLISION_H
#define MATH_COLLISION_H

#include "more_math.h"

typedef struct {
	vec3f p[4];
} face;
typedef struct {
	face f[6];
} hexahedron;

#define hexahedron_from_cube(side) (hexahedron_from_cuboid(side, side, side))
hexahedron hexahedron_from_cuboid(float s1, float s2, float s3);

#endif
