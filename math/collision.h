#ifndef MATH_COLLISION_H
#define MATH_COLLISION_H

#include "more_math.h"
#include "game/terrain.h"

float triangle_area_heron(vec3f a, vec3f b, vec3f c); // a, b, c are points, not edge vectors

float line_plane_intersect(line3f line, vec3f surf_norm, vec3f surf_p, vec3f* intersect, vec3f* collided_point);

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

hexahedron hexahedron_transform(const hexahedron* h, mat4f* transform_mat);

int hexahedron_check_projection_collision(const hexahedron* h1, const hexahedron* h2);
bbox3f hexahedron_get_bbox(const hexahedron* h);
int bbox_check_collision(const bbox3f* b1, const bbox3f* b2);

int bbox_check_terrain_collision(bbox3f bbox);
int hexahedron_check_collision(const hexahedron* h1, const hexahedron* h2, vec3f* resolution);
int hexahedron_check_terrain_collision(const hexahedron* h, vec3f* resolution, vec3f* new_rot); // resolution and new_rot can be NULL. If new_rot is not NULL, it should be passed a current rotation vector of hexahedron.

#endif
