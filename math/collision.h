#ifndef MATH_COLLISION_H
#define MATH_COLLISION_H

#include "more_math.h"

float line_plane_intersect(line3f line, vec3f surf_norm, vec3f surf_p, vec3f* intersect);

typedef struct {
	vec3f p[4];
} face;
typedef struct {
	face f[6];
} hexahedron;

#define hexahedron_from_cube(side) (hexahedron_from_cuboid(side, side, side))
hexahedron hexahedron_from_cuboid(float s1, float s2, float s3);

hexahedron hexahedron_transform(const hexahedron* h, mat4f* transform_mat);

int hexahedron_check_collision(const hexahedron* h1, const hexahedron* h2);

#endif
