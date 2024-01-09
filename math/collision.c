#include "math/collision.h"
#include <stddef.h>

float triangle_area_heron(vec3f a, vec3f b, vec3f c)
{
	vec3f side_a = vec3_sub(b, a);
	vec3f side_b = vec3_sub(c, b);
	vec3f side_c = vec3_sub(a, c);
	float ln_a = vec3_ln(side_a);
	float ln_b = vec3_ln(side_b);
	float ln_c = vec3_ln(side_c);
	float p = (ln_a + ln_b + ln_c) / 2; // perimeter
	return sqrt(p*(p - ln_a)*(p - ln_b)*(p - ln_c));
}

float line_plane_intersect(line3f line, vec3f surf_norm, vec3f surf_p, vec3f* intersect, vec3f* collided_point)
{ // source: https://stackoverflow.com/questions/5666222/3d-line-plane-intersection?noredirect=1&lq=1
	const float epsilon = 1e-6;
	vec3f u = vec3_sub(line.p2, line.p1);
	float dot = vec3_dot(surf_norm, u);
	if(fabs(dot) > epsilon){
		vec3f w = vec3_sub(line.p1, surf_p);
		float fac = -vec3_dot(surf_norm, w) / dot;
		u = vec3_smul(u, fac);
		vec3f inter = vec3_add(line.p1, u);
		if(intersect)
			*intersect = inter;
		if(collided_point){
			// determine which point of edge is closer to the plane
			float dist1 = vec3_ln(vec3_sub(inter, line.p1)),
			      dist2 = vec3_ln(vec3_sub(inter, line.p2));
			vec3f closest = dist1 < dist2 ? line.p1 : line.p2;
			*collided_point = closest;
		}
		return fac;
	}
	if(intersect)
		*intersect = (vec3f){NAN, NAN, NAN};
	return 0;
}

static int point_inside_cosurface_quad(vec3f p, face f)
{ // source: https://stackoverflow.com/questions/5922027/how-to-determine-if-a-point-is-within-a-quadrilateral
	// TODO pre-calculate edge lengths and use them without any external functions to optimize
	float quad_area = triangle_area_heron(f.p[0], f.p[1], f.p[2]) + triangle_area_heron(f.p[2], f.p[3], f.p[0]);
	float tri_area_sum = 0;
	for(size_t i = 0; i < 4; ++i)
		tri_area_sum += triangle_area_heron(f.p[i], f.p[i == 3 ? 0 : i+1], p);
	return tri_area_sum == quad_area;
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

#define SET_MIN(where, what) {if((what) < (where)) (where) = (what);}
#define SET_MAX(where, what) {if((what) > (where)) (where) = (what);}
int hexahedron_check_projection_collision(const hexahedron* h1, const hexahedron* h2)
{
	vec3f min1 = {INFINITY, INFINITY, INFINITY}, max1 = {-INFINITY, -INFINITY, -INFINITY};
	vec3f min2 = {INFINITY, INFINITY, INFINITY}, max2 = {-INFINITY, -INFINITY, -INFINITY};
	for(size_t _f = 0; _f < 6; ++_f)
		for(size_t _p = 0; _p < 4; ++_p){
			SET_MIN(min1.x, h1->f[_f].p[_p].x);
			SET_MIN(min1.y, h1->f[_f].p[_p].y);
			SET_MIN(min1.z, h1->f[_f].p[_p].z);
			SET_MAX(max1.x, h1->f[_f].p[_p].x);
			SET_MAX(max1.y, h1->f[_f].p[_p].y);
			SET_MAX(max1.z, h1->f[_f].p[_p].z);
		}
	for(size_t _f = 0; _f < 6; ++_f)
		for(size_t _p = 0; _p < 4; ++_p){
			SET_MIN(min2.x, h2->f[_f].p[_p].x);
			SET_MIN(min2.y, h2->f[_f].p[_p].y);
			SET_MIN(min2.z, h2->f[_f].p[_p].z);
			SET_MAX(max2.x, h2->f[_f].p[_p].x);
			SET_MAX(max2.y, h2->f[_f].p[_p].y);
			SET_MAX(max2.z, h2->f[_f].p[_p].z);
		}
	return !(max1.x < min2.x || max2.x < min1.x)
	       && !(max1.y < min2.y || max2.y < min1.y)
	       && !(max1.z < min2.z || max2.z < min1.z);
}

int hexahedron_check_collision(const hexahedron* h1, const hexahedron* h2, vec3f* _vel)
{
	vec3f vel = *_vel;
	int collided1 = _hexahedron_check_collision(h1, h2, &vel);
	if(collided1 == -1)
		return 0;
	if(collided1){
		*_vel = vel;
		return collided1;
	}
	vel = *_vel;
	int collided2 = _hexahedron_check_collision(h2, h1, &vel);
	if(collided2 == -1)
		return 0;
	if(collided2){
		*_vel = vel;
		return collided2;
	}
	return 0;
}

int _hexahedron_check_collision(const hexahedron* h1, const hexahedron* h2, vec3f* vel)
{
	if(!hexahedron_check_projection_collision(h1, h2))
		return -1; // cannot possibly overlap if projections dont
	int collided = 0; vec3f resolution = {0, 0, 0};
	vec3f intersec, collision_p;
	for(size_t _f = 0; _f < 6; ++_f) // iterate over h1 faces
		for(size_t _f2 = 0; _f2 < 6; ++_f2){ // iterate over h2 faces
			vec3f e1 = vec3_sub(h2->f[_f2].p[1], h2->f[_f2].p[0]);
			vec3f e2 = vec3_sub(h2->f[_f2].p[2], h2->f[_f2].p[1]);
			vec3f surf_norm = vec3_cross(e2, e1), surf_p = h2->f[_f2].p[0];
			for(size_t _p = 0; _p < 4; ++_p){ // iterate over h1 face's edges
				line3f edge = {h1->f[_f].p[_p], h1->f[_f].p[_p == 3 ? 0 : _p + 1]};
				float fac = line_plane_intersect(edge, surf_norm, surf_p, &intersec, &collision_p);
				if(fac >= 0 && fac <= 1 && !isnan(intersec.x) && point_inside_cosurface_quad(intersec, h2->f[_f2])){
					collided = 1;
					goto collision_end;
				}
			}
		}
collision_end:
	if(collided && vec3_ln(*vel) > 0){ // point on h1 collided with a surface on h2
		float velx_sign = sign(vel->x), vely_sign = sign(vel->y), velz_sign = sign(vel->z);
		vec3f init_collision_p = {INFINITY * velx_sign, INFINITY * vely_sign, INFINITY * velz_sign}, init_collision_p2 = init_collision_p; // find the earliest point of collision amongst h2 vertices
		for(size_t _f = 0; _f < 6; ++_f) // iterate over all h2 points TODO optimize
			for(size_t _p = 0; _p < 4; ++_p){
				vec3f pt = h2->f[_f].p[_p];
				if( ((velx_sign == 1 && pt.x < init_collision_p2.x) || (velx_sign == -1 && pt.x > init_collision_p2.x) || (velx_sign == 0)) &&
					((vely_sign == 1 && pt.y < init_collision_p2.y) || (vely_sign == -1 && pt.y > init_collision_p2.y) || (vely_sign == 0)) &&
					((velz_sign == 1 && pt.z < init_collision_p2.z) || (velz_sign == -1 && pt.z > init_collision_p2.z) || (velz_sign == 0))){
					if( ((velx_sign == 1 && pt.x < init_collision_p.x) || (velx_sign == -1 && pt.x > init_collision_p.x) || (velx_sign == 0)) &&
						((vely_sign == 1 && pt.y < init_collision_p.y) || (vely_sign == -1 && pt.y > init_collision_p.y) || (vely_sign == 0)) &&
						((velz_sign == 1 && pt.z < init_collision_p.z) || (velz_sign == -1 && pt.z > init_collision_p.z) || (velz_sign == 0))){
						init_collision_p2 = init_collision_p;
						init_collision_p = pt;
					}
					else {
						init_collision_p2 = pt;
					}
				}
			}
		vec3f target_p = perpendicular_3f((line3f){init_collision_p, init_collision_p2}, intersec);

		float min_fac = INFINITY;
		for(size_t _f = 0; _f < 6; ++_f){ // iterate over all h1 faces
			vec3f test;
			vec3f e1 = vec3_sub(h1->f[_f].p[1], h1->f[_f].p[0]);
			vec3f e2 = vec3_sub(h1->f[_f].p[2], h1->f[_f].p[1]);
			vec3f surf_norm = vec3_cross(e2, e1), surf_p = h1->f[_f].p[0];
			float fac = line_plane_intersect((line3f){target_p, vec3_sub(target_p, *vel)}, surf_norm, surf_p, &test, NULL);
			if(fac < min_fac && !isnan(test.x) && fac > 0){
				resolution = vec3_sub(target_p, test);
				min_fac = fac;
			}
		}

		// check for better resolutions that are done by moving h2; if they are found, don't resolve
		velx_sign *= -1; vely_sign *= -1; velz_sign *= -1;
		init_collision_p = (vec3f){INFINITY * velx_sign, INFINITY * vely_sign, INFINITY * velz_sign};
	       	init_collision_p2 = init_collision_p; // find the earliest point of collision amongst h1 vertices
		for(size_t _f = 0; _f < 6; ++_f) // iterate over all h1 points TODO optimize
			for(size_t _p = 0; _p < 4; ++_p){
				vec3f pt = h1->f[_f].p[_p];
				if( ((velx_sign == 1 && pt.x < init_collision_p2.x) || (velx_sign == -1 && pt.x > init_collision_p2.x) || (velx_sign == 0)) &&
					((vely_sign == 1 && pt.y < init_collision_p2.y) || (vely_sign == -1 && pt.y > init_collision_p2.y) || (vely_sign == 0)) &&
					((velz_sign == 1 && pt.z < init_collision_p2.z) || (velz_sign == -1 && pt.z > init_collision_p2.z) || (velz_sign == 0))){
					if( ((velx_sign == 1 && pt.x < init_collision_p.x) || (velx_sign == -1 && pt.x > init_collision_p.x) || (velx_sign == 0)) &&
						((vely_sign == 1 && pt.y < init_collision_p.y) || (vely_sign == -1 && pt.y > init_collision_p.y) || (vely_sign == 0)) &&
						((velz_sign == 1 && pt.z < init_collision_p.z) || (velz_sign == -1 && pt.z > init_collision_p.z) || (velz_sign == 0))){
						init_collision_p2 = init_collision_p;
						init_collision_p = pt;
					}
					else {
						init_collision_p2 = pt;
					}
				}
			}
		target_p = perpendicular_3f((line3f){init_collision_p, init_collision_p2}, intersec);

		for(size_t _f = 0; _f < 6; ++_f){ // iterate over all h2 faces
			vec3f test;
			vec3f e1 = vec3_sub(h2->f[_f].p[1], h2->f[_f].p[0]);
			vec3f e2 = vec3_sub(h2->f[_f].p[2], h2->f[_f].p[1]);
			vec3f surf_norm = vec3_cross(e2, e1), surf_p = h2->f[_f].p[0];
			float fac = line_plane_intersect((line3f){target_p, vec3_sub(target_p, *vel)}, surf_norm, surf_p, &test, NULL);
			if(fac < min_fac && !isnan(test.x) && fac > 0){
				resolution = (vec3f){0, 0, 0};
				break;
			}
		}

	}
	*vel = resolution;
	return collided;
}
