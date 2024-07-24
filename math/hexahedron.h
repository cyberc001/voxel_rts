#ifndef MATH_HEXAHEDRON_H
#define MATH_HEXAHEDRON_H

#include "more_math.h"
#include "math/body.h"

typedef struct {
	vec3f p[4];
} face3f;
typedef struct {
	face3f f[6];
} hexahedron;

#define hexahedron_from_cube(side) (hexahedron_from_cuboid(side, side, side))
hexahedron hexahedron_from_cuboid(float s1, float s2, float s3);
#define hexahedron_from_cube_centered(side) (hexahedron_from_cuboid_centered(side, side, side))
hexahedron hexahedron_from_cuboid_centered(float s1, float s2, float s3);
typedef struct terrain_piece terrain_piece;
hexahedron hexahedron_from_terrain_piece(unsigned tx, unsigned ty, terrain_piece* piece);

vec3f hexahedron_get_center(const hexahedron* h1);
bbox3f hexahedron_get_bbox(const hexahedron* h);

hexahedron hexahedron_transform(const hexahedron* h, mat4f* transform_mat);

/* body */
typedef struct {
	struct body;
	hexahedron geom, geom_cache;
} body_hexahedron;

vec3f hexahedron_get_vertice(const hexahedron* h, size_t idx);
axes_list hexahedron_get_separating_axes_and_edges(const hexahedron* h, size_t* normals_cnt);

#endif
