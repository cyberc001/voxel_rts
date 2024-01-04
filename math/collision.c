#include "math/collision.h"
#include <stddef.h>

float line_plane_intersect(line3f line, vec3f surf_norm, vec3f surf_p, vec3f* intersect)
{ // source: https://stackoverflow.com/questions/5666222/3d-line-plane-intersection?noredirect=1&lq=1
	const float epsilon = 1e-6;
	vec3f u = vec3_sub(line.p2, line.p1);
	float dot = vec3_dot(surf_norm, u);

	if(fabs(dot) > epsilon){
		vec3f w = vec3_sub(line.p1, surf_p);
		float fac = -vec3_dot(surf_norm, w) / dot;
		u = vec3_smul(u, fac);
		if(intersect)
			*intersect = vec3_add(line.p1, u);
		return fac;
	}
	if(intersect)
		*intersect = (vec3f){NAN, NAN, NAN};
	return 0;
}


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

hexahedron hexahedron_transform(const hexahedron* h, mat4f* transform_mat)
{
	hexahedron _out;
	for(size_t i = 0; i < 6; ++i)
		for(size_t j = 0; j < 4; ++j)
			_out.f[i].p[j] = mat4f_mul_vec3f(transform_mat, h->f[i].p[j]);
	return _out;
}

int hexahedron_check_collision(const hexahedron* h1, const hexahedron* h2)
{
	for(size_t _f = 0; _f < 6; ++_f) // iterate over h1 faces
		for(size_t _f2 = 0; _f2 < 6; ++_f2){ // iterate over h2 faces
			vec3f e1 = vec3_sub(h2->f[_f2].p[1], h2->f[_f2].p[0]);
			vec3f e2 = vec3_sub(h2->f[_f2].p[2], h2->f[_f2].p[1]);
			vec3f surf_norm = vec3_cross(e2, e1);
			for(size_t _p = 0; _p < 4; ++_p){ // iterate over h1 face's edges
				line3f edge = {h1->f[_f].p[_p], h1->f[_f].p[_p == 3 ? 0 : _p + 1]};
				float fac = line_plane_intersect(edge, surf_norm, h2->f[_f2].p[0], NULL);
				printf("edge:\n"); vec3f_print(edge.p1); vec3f_print(edge.p2);
				printf("surface:\n"); vec3f_print(h2->f[_f2].p[0]); vec3f_print(surf_norm);
				vec3f_print(e1); vec3f_print(e2);
				printf("factor: %f\n", fac);
				if(fac >= 0 && fac <= 1)
					return 1;

			}
		}
	return 0;
}
