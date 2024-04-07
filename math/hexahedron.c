#include "math/hexahedron.h"
#include <stddef.h>
#include "game/terrain.h"

hexahedron hexahedron_from_cuboid(float s1, float s2, float s3)
{
	hexahedron h;
	h.f[0].p[0] = (vec3f){0, 0, 0}; h.f[0].p[1] = (vec3f){s1, 0, 0};
	h.f[0].p[2] = (vec3f){s1, s2, 0}; h.f[0].p[3] = (vec3f){0, s2, 0};
	h.f[1].p[0] = (vec3f){0, s2, s3}; h.f[1].p[1] = (vec3f){s1, s2, s3};
	h.f[1].p[2] = (vec3f){s1, 0, s3}; h.f[1].p[3] = (vec3f){0, 0, s3};
	h.f[2].p[0] = (vec3f){s1, s2, s3}; h.f[2].p[1] = (vec3f){s1, 0, s3};
	h.f[2].p[2] = (vec3f){s1, 0, 0}; h.f[1].p[3] = (vec3f){s1, s2, 0};
	h.f[3].p[0] = (vec3f){0, s2, s3}; h.f[3].p[1] = (vec3f){0, 0, s3};
	h.f[3].p[2] = (vec3f){0, 0, 0}; h.f[3].p[3] = (vec3f){0, s2, 0};
	h.f[4].p[0] = (vec3f){0, s2, 0}; h.f[4].p[1] = (vec3f){s1, s2, 0};
	h.f[4].p[2] = (vec3f){s1, s2, s3}; h.f[4].p[3] = (vec3f){0, s2, s3};
	h.f[5].p[0] = (vec3f){0, 0, 0}; h.f[5].p[1] = (vec3f){s1, 0, 0};
	h.f[5].p[2] = (vec3f){s1, 0, s3}; h.f[5].p[3] = (vec3f){0, 0, s3};
	return h;
}
hexahedron hexahedron_from_cuboid_centered(float s1, float s2, float s3)
{
	hexahedron h;
	h.f[0].p[0] = (vec3f){-s1/2, -s2/2, -s3/2}; h.f[0].p[1] = (vec3f){s1/2, -s2/2, -s3/2};
	h.f[0].p[2] = (vec3f){s1/2, s2/2, -s3/2}; h.f[0].p[3] = (vec3f){-s1/2, s2/2, -s3/2};
	h.f[1].p[0] = (vec3f){-s1/2, s2/2, s3/2}; h.f[1].p[1] = (vec3f){s1/2, s2/2, s3/2};
	h.f[1].p[2] = (vec3f){s1/2, -s2/2, s3/2}; h.f[1].p[3] = (vec3f){-s1/2, -s2/2, s3/2};
	h.f[2].p[0] = (vec3f){s1/2, s2/2, s3/2}; h.f[2].p[1] = (vec3f){s1/2, -s2/2, s3/2};
	h.f[2].p[2] = (vec3f){s1/2, -s2/2, -s3/2}; h.f[1].p[3] = (vec3f){s1/2, s2/2, -s3/2};
	h.f[3].p[0] = (vec3f){-s1/2, s2/2, s3/2}; h.f[3].p[1] = (vec3f){-s1/2, -s2/2, s3/2};
	h.f[3].p[2] = (vec3f){-s1/2, -s2/2, -s3/2}; h.f[3].p[3] = (vec3f){-s1/2, s2/2, -s3/2};
	h.f[4].p[0] = (vec3f){-s1/2, s2/2, -s3/2}; h.f[4].p[1] = (vec3f){s1/2, s2/2, -s3/2};
	h.f[4].p[2] = (vec3f){s1/2, s2/2, s3/2}; h.f[4].p[3] = (vec3f){-s1/2, s2/2, s3/2};
	h.f[5].p[0] = (vec3f){-s1/2, -s2/2, -s3/2}; h.f[5].p[1] = (vec3f){s1/2, -s2/2, -s3/2};
	h.f[5].p[2] = (vec3f){s1/2, -s2/2, s3/2}; h.f[5].p[3] = (vec3f){-s1/2, -s2/2, s3/2};
	return h;
}
hexahedron hexahedron_from_terrain_piece(unsigned tx, unsigned ty, terrain_piece* piece)
{
	hexahedron h;
	h.f[0].p[0] = (vec3f){tx, piece->z_floor[0], ty};
	h.f[0].p[1] = (vec3f){tx + 1, piece->z_floor[1], ty};
	h.f[0].p[2] = (vec3f){tx + 1, piece->z_floor[2], ty + 1};
	h.f[0].p[3] = (vec3f){tx, piece->z_floor[3], ty + 1};
	h.f[1].p[0] = (vec3f){tx, piece->z_floor[0], ty};
	h.f[1].p[1] = (vec3f){tx + 1, piece->z_floor[1], ty};
	h.f[1].p[2] = (vec3f){tx + 1, piece->z_ceil[1], ty};
	h.f[1].p[3] = (vec3f){tx, piece->z_ceil[0], ty};
	h.f[2].p[0] = (vec3f){tx, piece->z_floor[3], ty + 1};
	h.f[2].p[1] = (vec3f){tx + 1, piece->z_floor[2], ty + 1};
	h.f[2].p[2] = (vec3f){tx + 1, piece->z_ceil[2], ty + 1};
	h.f[2].p[3] = (vec3f){tx, piece->z_ceil[3], ty + 1};
	h.f[3].p[0] = (vec3f){tx, piece->z_floor[0], ty};
	h.f[3].p[1] = (vec3f){tx, piece->z_floor[3], ty + 1};
	h.f[3].p[2] = (vec3f){tx, piece->z_ceil[3], ty + 1};
	h.f[3].p[3] = (vec3f){tx, piece->z_ceil[0], ty};
	h.f[4].p[0] = (vec3f){tx + 1, piece->z_floor[1], ty};
	h.f[4].p[1] = (vec3f){tx + 1, piece->z_floor[2], ty + 1};
	h.f[4].p[2] = (vec3f){tx + 1, piece->z_ceil[2], ty + 1};
	h.f[4].p[3] = (vec3f){tx + 1, piece->z_ceil[1], ty};
	h.f[5].p[0] = (vec3f){tx, piece->z_ceil[0], ty};
	h.f[5].p[1] = (vec3f){tx + 1, piece->z_ceil[1], ty};
	h.f[5].p[2] = (vec3f){tx + 1, piece->z_ceil[2], ty + 1};
	h.f[5].p[3] = (vec3f){tx, piece->z_ceil[3], ty + 1};
	return h;
}


vec3f hexahedron_get_center(const hexahedron* h1)
{
	vec3f c = {0, 0, 0};
	for(size_t _f = 0; _f < 6; ++_f)
		for(size_t _p = 0; _p < 4; ++_p)
			c = vec3_add(c, h1->f[_f].p[_p]);
	return vec3_sdiv(c, 24);
}
#define SET_MIN(where, what) {if((what) < (where)) (where) = (what);}
#define SET_MAX(where, what) {if((what) > (where)) (where) = (what);}
bbox3f hexahedron_get_bbox(const hexahedron* h)
{
	vec3f _min = {INFINITY, INFINITY, INFINITY}, _max = {-INFINITY, -INFINITY, -INFINITY};
	for(size_t _f = 0; _f < 6; ++_f)
		for(size_t _p = 0; _p < 4; ++_p){
			SET_MIN(_min.x, h->f[_f].p[_p].x);
			SET_MIN(_min.y, h->f[_f].p[_p].y);
			SET_MIN(_min.z, h->f[_f].p[_p].z);
			SET_MAX(_max.x, h->f[_f].p[_p].x);
			SET_MAX(_max.y, h->f[_f].p[_p].y);
			SET_MAX(_max.z, h->f[_f].p[_p].z);
		}
	return (bbox3f){_min, _max};
}


hexahedron hexahedron_transform(const hexahedron* h, mat4f* transform_mat)
{
	hexahedron _out;
	for(size_t i = 0; i < 6; ++i)
		for(size_t j = 0; j < 4; ++j)
			_out.f[i].p[j] = mat4f_mul_vec3f(transform_mat, h->f[i].p[j]);
	return _out;
}

