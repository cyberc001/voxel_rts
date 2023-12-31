#ifndef MATH_COLLISION_H
#define MATH_COLLISION_H

#include "more_math.h"

float triangle_area_heron(vec3f a, vec3f b, vec3f c); // a, b, c are points, not edge vectors

float line_plane_intersect(line3f line, vec3f surf_norm, vec3f surf_p, vec3f* intersect, vec3f* collided_point);

typedef struct {
	vec3f p[4];
} face;
typedef struct {
	face f[6];
} hexahedron;

#define hexahedron_from_cube(side) (hexahedron_from_cuboid(side, side, side))
hexahedron hexahedron_from_cuboid(float s1, float s2, float s3);

hexahedron hexahedron_transform(const hexahedron* h, mat4f* transform_mat);

/* vel - if not NULL, should point to hexahedron body's velocity. Resolution vector is written to it. */
int hexahedron_check_collision(const hexahedron* h1, const hexahedron* h2, vec3f* vel);
#endif
