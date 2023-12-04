#include "math/collision.h"

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
