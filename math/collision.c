#include "math/collision.h"
#include <stddef.h>
#include "dyn_array.h"

float triangle2_area_heron(vec2f a, vec2f b, vec2f c)
{
	vec2f side_a = vec2_sub(b, a);
	vec2f side_b = vec2_sub(c, b);
	vec2f side_c = vec2_sub(a, c);
	float ln_a = vec2_ln(side_a);
	float ln_b = vec2_ln(side_b);
	float ln_c = vec2_ln(side_c);
	float p = (ln_a + ln_b + ln_c) / 2; // perimeter
	return sqrt(p*(p - ln_a)*(p - ln_b)*(p - ln_c));
}
float triangle3_area_heron(vec3f a, vec3f b, vec3f c)
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

int point_is_in_face2(vec2f pt, const face2f* f)
{
	float face_area = triangle2_area_heron(f->p[0], f->p[1], f->p[2]) + triangle2_area_heron(f->p[0], f->p[3], f->p[2]);
	float pt_area = 0;
	for(size_t i = 0; i < 4; ++i)
		pt_area += triangle2_area_heron(f->p[i], f->p[i == 3 ? 0 : i+1], pt);
	return eqf(face_area, pt_area);
}
int point_is_in_face3(vec3f pt, const face3f* f)
{
	float face_area = triangle3_area_heron(f->p[0], f->p[1], f->p[2]) + triangle3_area_heron(f->p[0], f->p[3], f->p[2]);
	float pt_area = 0;
	for(size_t i = 0; i < 4; ++i)
		pt_area += triangle3_area_heron(f->p[i], f->p[i == 3 ? 0 : i+1], pt);
	return eqf(face_area, pt_area);
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
int bbox_check_collision(const bbox3f* b1, const bbox3f* b2)
{
	return !(b1->max.x < b2->min.x || b2->max.x < b1->min.x)
	       && !(b1->max.y < b2->min.y || b2->max.y < b1->min.y)
	       && !(b1->max.z < b2->min.z || b2->max.z < b1->min.z);
}

/* SAT */

static vec2f hexahedron_project_on_axis(const hexahedron* h1, vec3f axis)
{
	float min = vec3_dot(axis, h1->f[0].p[0]), max = min;
	for(size_t _p = 1; _p < 4; ++_p){
		float proj = vec3_dot(axis, h1->f[0].p[_p]);
		if(proj < min) min = proj;
		else if(proj > max) max = proj;
	}
	for(size_t _f = 1; _f < 6; ++_f){
		for(size_t _p = 0; _p < 4; ++_p){
			float proj = vec3_dot(axis, h1->f[_f].p[_p]);
			if(proj < min) min = proj;
				else if(proj > max) max = proj;
		}
	}
	return (vec2f){min, max};
}
static int do_proj_overlap(vec2f proj1, vec2f proj2)
{
	return proj1.x <= proj2.y && proj1.y >= proj2.x;
}
static float get_proj_overlap(vec2f proj1, vec2f proj2)
{
	return min(proj1.y, proj2.y) - max(proj1.x, proj2.x);
}
static size_t hexahedron_get_separating_axes(const hexahedron* h1, const hexahedron* h2, vec3f* axis_arr)
{
	size_t arr_i = 0;
	// get normals to h1 and h2 faces
	for(size_t _f = 0; _f < 6; ++_f){
		vec3f edge1 = vec3_sub(h1->f[_f].p[1], h1->f[_f].p[0]);
		vec3f edge2 = vec3_sub(h1->f[_f].p[2], h1->f[_f].p[1]);
		vec3f norm = vec3_norm(vec3_cross(edge1, edge2));
		int insert = 1;
		for(size_t i = 0; i < arr_i; ++i)
			if(vec3_eq(axis_arr[i], norm))
			{ insert = 0; break; }
		if(insert)
			axis_arr[arr_i++] = norm;

		edge1 = vec3_sub(h2->f[_f].p[1], h2->f[_f].p[0]);
		edge2 = vec3_sub(h2->f[_f].p[2], h2->f[_f].p[1]);
		norm = vec3_norm(vec3_cross(edge1, edge2));
		insert = 1;
		for(size_t i = 0; i < arr_i; ++i)
			if(vec3_eq(axis_arr[i], norm))
			{ insert = 0; break; }
		if(insert)
			axis_arr[arr_i++] = norm;
	}
	// get all edge-to-edge cases (i.e. cross-products of edges)
	for(size_t _f = 0; _f < 6; ++_f){
		for(size_t _p = 0; _p < 4; ++_p){
			for(size_t _f2 = 0; _f2 < 6; ++_f2){
				for(size_t _p2 = 0; _p2 < 4; ++_p2){
					vec3f e1 = h1->f[_f].p[_p], e2 = h1->f[_f].p[_p == 3 ? 0 : _p + 1],
					e3 = h2->f[_f2].p[_p2], e4 = h2->f[_f2].p[_p2 == 3 ? 0 : _p2 + 1];
					vec3f edge1 = vec3_sub(e1, e2), edge2 = vec3_sub(e3, e4);
					vec3f edge = vec3_cross(edge1, edge2);
					vec3f norm = vec3_norm(edge);
					if(!isnan(norm.x) && !isnan(norm.y) && !isnan(norm.z)){
						int insert = 1;
						for(size_t i = 0; i < arr_i; ++i)
							if(vec3_eq(axis_arr[i], norm))
							{ insert = 0; break; }
						if(insert)
							axis_arr[arr_i++] = norm;
					}
				}
			}
		}
	}
	return arr_i;
}

int hexahedron_check_collision(const hexahedron* h1, const hexahedron* h2, vec3f* resol)
{
	vec3f axes[6 + 6 + 576];
	if(!hexahedron_check_projection_collision(h1, h2))
		return 0;
	size_t axes_ln = hexahedron_get_separating_axes(h1, h2, axes);
	float overlap = INFINITY;
	vec3f resolution;

	for(size_t i = 0; i < axes_ln; ++i){
		vec3f axis = axes[i];
		if(vec3_ln(axis) == 0 || isnan(axis.x))
			continue;
		vec2f proj1 = hexahedron_project_on_axis(h1, axis),
		      proj2 = hexahedron_project_on_axis(h2, axis);
		if(do_proj_overlap(proj1, proj2)){
			float o = get_proj_overlap(proj1, proj2);
			if(o < overlap){
				overlap = o;
				resolution = axis;
			}
		}
		else
			return 0;
	}

	resolution = vec3_smul(resolution, overlap);
	vec3f p1 = hexahedron_get_center(h1), p2 = hexahedron_get_center(h2);
	vec3f vd = vec3_sub(p1, p2);
	if(vec3_dot(vd, resolution) > 0)
		resolution = vec3_smul(resolution, -1);
	*resol = resolution;
	return 1;
}

int bbox_check_terrain_collision(bbox3f bbox)
{
	vec3f terrain_min = {0, 0, 0};
	vec3_setmin(bbox.min, terrain_min);
	vec3_setmin(bbox.max, terrain_min);

	uint32_t beg_tx = floor(bbox.min.x / TERRAIN_PIECE_SIZE),
		 end_tx = ceil(bbox.max.x / TERRAIN_PIECE_SIZE);
	uint32_t beg_ty = floor(bbox.min.z / TERRAIN_PIECE_SIZE),
		 end_ty = ceil(bbox.max.z / TERRAIN_PIECE_SIZE);
	for(uint32_t tx = beg_tx; tx <= end_tx; ++tx)
		for(uint32_t ty = beg_ty; ty <= end_ty; ++ty){
			terrain_piece* piece = terrain_get_piece(tx, ty);

			while(piece){
				bbox3f tpiece_bbox;
				tpiece_bbox.min.x = tx; tpiece_bbox.min.z = ty;
				tpiece_bbox.max.x = tx + 1; tpiece_bbox.max.z = ty + 1;

				tpiece_bbox.min.y = INFINITY;
				for(size_t i = 0; i < 4; ++i)
					if(piece->z_floor[i] < tpiece_bbox.min.y)
						tpiece_bbox.min.y = piece->z_floor[i];
				tpiece_bbox.max.y = -INFINITY;
				for(size_t i = 0; i < 4; ++i)
					if(piece->z_ceil[i] > tpiece_bbox.max.y)
						tpiece_bbox.max.y = piece->z_ceil[i];

				tpiece_bbox.min = vec3_smul(tpiece_bbox.min, TERRAIN_PIECE_SIZE);
				tpiece_bbox.max = vec3_smul(tpiece_bbox.max, TERRAIN_PIECE_SIZE);

				if(tpiece_bbox.max.y - bbox.min.y > 0.1 && bbox_check_collision(&bbox, &tpiece_bbox)){
					printf("BBOX\n");
					vec3f_print(bbox.min);
					vec3f_print(bbox.max);
					printf("COLLIDED WITH\n");
					vec3f_print(tpiece_bbox.min);
					vec3f_print(tpiece_bbox.max);
					return 1;
				}
				piece = piece->next;
			}
		}
	return 0;
}

int hexahedron_check_terrain_collision(const hexahedron* h, vec3f* resolution, vec3f* new_rot)
{
	vec3f orig_rot;
	if(new_rot){
		orig_rot = *new_rot;
		*new_rot = (vec3f){NAN, NAN, NAN};
	}

	bbox3f bbox = hexahedron_get_bbox(h);
	vec3f terrain_min = {0, 0, 0};
	vec3_setmin(bbox.min, terrain_min);
	vec3_setmin(bbox.max, terrain_min);

	uint32_t beg_tx = floor(bbox.min.x / TERRAIN_PIECE_SIZE),
		 end_tx = ceil(bbox.max.x / TERRAIN_PIECE_SIZE);
	uint32_t beg_ty = floor(bbox.min.z / TERRAIN_PIECE_SIZE),
		 end_ty = ceil(bbox.max.z / TERRAIN_PIECE_SIZE);
	int collided = 0;
	vec3f max_resol = {0, 0, 0};
	for(uint32_t tx = beg_tx; tx <= end_tx; ++tx)
		for(uint32_t ty = beg_ty; ty <= end_ty; ++ty){
			terrain_piece* piece = terrain_get_piece(tx, ty);

			while(piece){
				hexahedron tpiece_h;
	
				tpiece_h.f[0].p[0] = (vec3f){tx, piece->z_floor[0], ty};
				tpiece_h.f[0].p[1] = (vec3f){tx + 1, piece->z_floor[1], ty};
				tpiece_h.f[0].p[2] = (vec3f){tx + 1, piece->z_floor[2], ty + 1};
				tpiece_h.f[0].p[3] = (vec3f){tx, piece->z_floor[3], ty + 1};
				tpiece_h.f[1].p[0] = (vec3f){tx, piece->z_floor[0], ty};
				tpiece_h.f[1].p[1] = (vec3f){tx + 1, piece->z_floor[1], ty};
				tpiece_h.f[1].p[2] = (vec3f){tx + 1, piece->z_ceil[1], ty};
				tpiece_h.f[1].p[3] = (vec3f){tx, piece->z_ceil[0], ty};
				tpiece_h.f[2].p[0] = (vec3f){tx, piece->z_floor[3], ty + 1};
				tpiece_h.f[2].p[1] = (vec3f){tx + 1, piece->z_floor[2], ty + 1};
				tpiece_h.f[2].p[2] = (vec3f){tx + 1, piece->z_ceil[2], ty + 1};
				tpiece_h.f[2].p[3] = (vec3f){tx, piece->z_ceil[3], ty + 1};
				tpiece_h.f[3].p[0] = (vec3f){tx, piece->z_floor[0], ty};
				tpiece_h.f[3].p[1] = (vec3f){tx, piece->z_floor[3], ty + 1};
				tpiece_h.f[3].p[2] = (vec3f){tx, piece->z_ceil[3], ty + 1};
				tpiece_h.f[3].p[3] = (vec3f){tx, piece->z_ceil[0], ty};
				tpiece_h.f[4].p[0] = (vec3f){tx + 1, piece->z_floor[1], ty};
				tpiece_h.f[4].p[1] = (vec3f){tx + 1, piece->z_floor[2], ty + 1};
				tpiece_h.f[4].p[2] = (vec3f){tx + 1, piece->z_ceil[2], ty + 1};
				tpiece_h.f[4].p[3] = (vec3f){tx + 1, piece->z_ceil[1], ty};
				tpiece_h.f[5].p[0] = (vec3f){tx, piece->z_ceil[0], ty};
				tpiece_h.f[5].p[1] = (vec3f){tx + 1, piece->z_ceil[1], ty};
				tpiece_h.f[5].p[2] = (vec3f){tx + 1, piece->z_ceil[2], ty + 1};
				tpiece_h.f[5].p[3] = (vec3f){tx, piece->z_ceil[3], ty + 1};

				for(size_t _f = 0; _f < 6; ++_f)
					for(size_t _p = 0; _p < 4; ++_p)
						tpiece_h.f[_f].p[_p] = vec3_smul(tpiece_h.f[_f].p[_p], TERRAIN_PIECE_SIZE);

				vec3f resol;
				int _collided = hexahedron_check_collision(h, &tpiece_h, &resol);
				if(_collided){
					if(vec3_ln(resol) > vec3_ln(max_resol)){
						max_resol = resol;
						vec3f e1 = vec3_sub(tpiece_h.f[5].p[0], tpiece_h.f[5].p[1]), e2 = vec3_sub(tpiece_h.f[5].p[1], tpiece_h.f[5].p[2]);
						vec3f norm = vec3_cross(e1, e2);

						if(new_rot){
							vec3f vel = {0, -1, 0};
							vel = vec3f_rot(vel, orig_rot);
							*new_rot = vec3f_get_rot_between(vel, norm);
						}
					}
					collided = 1;
				}
				piece = piece->next;
			}
		}
	if(resolution)
		*resolution = max_resol;
	return collided;
}
