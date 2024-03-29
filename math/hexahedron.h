#ifndef MATH_HEXAHEDRON_H
#define MATH_HEXAHEDRON_H

#include "more_math.h"

typedef struct {
	vec3f p[4];
} face3f;
typedef struct {
	face3f f[6];
} hexahedron;

typedef struct {
	vec3f min, max;
} bbox3f;

#define hexahedron_from_cube(side) (hexahedron_from_cuboid(side, side, side))
hexahedron hexahedron_from_cuboid(float s1, float s2, float s3);
#define hexahedron_from_cube_centered(side) (hexahedron_from_cuboid_centered(side, side, side))
hexahedron hexahedron_from_cuboid_centered(float s1, float s2, float s3);

vec3f hexahedron_get_center(const hexahedron* h1);
bbox3f hexahedron_get_bbox(const hexahedron* h);

hexahedron hexahedron_transform(const hexahedron* h, mat4f* transform_mat);

#endif
