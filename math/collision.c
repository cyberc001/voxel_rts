#include "math/collision.h"
#include <stddef.h>
#include "dyn_array.h"
#include "math/quat.h"

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
int bbox_contains_bbox(const bbox3f* container, const bbox3f* containee)
{
	return in_range(containee->min.x, container->min.x, container->max.x)
	       && in_range(containee->max.x, container->min.x, container->max.x)
	       && in_range(containee->min.y, container->min.y, container->max.y)
	       && in_range(containee->max.y, container->min.y, container->max.y)
	       && in_range(containee->min.z, container->min.z, container->max.z)
	       && in_range(containee->max.z, container->min.z, container->max.z);
}

/* SAT */

static int do_proj_overlap(vec2f proj1, vec2f proj2)
{
	return proj1.x <= proj2.y && proj1.y >= proj2.x;
}
static float get_proj_overlap(vec2f proj1, vec2f proj2)
{
	return min(proj1.y, proj2.y) - max(proj1.x, proj2.x);
}
static int does_proj_contain(vec2f proj, vec2f tocheck)
{ // assumes that proj and tocheck overlap
	return tocheck.x >= proj.x && tocheck.y <= proj.y;
}

int sat_check_collision(body* b1, body* b2, vec3f* resol)
{
	size_t normals_cnt1, normals_cnt2;
	axes_list axes1 = body_get_separating_axes_and_edges(b1, &normals_cnt1),
		  axes2 = body_get_separating_axes_and_edges(b2, &normals_cnt2);
	vec3f center1 = body_get_center(b1),
	      center2 = body_get_center(b2);

	axes_list cross_axes;
	axes_list_create(&cross_axes);
	for(size_t i = normals_cnt1; i < axes1.busy; ++i)
		for(size_t j = normals_cnt2; j < axes2.busy; ++j){
			vec3f cross = vec3_cross(axes1.data[i], axes2.data[j]);
			vec3f norm = vec3_norm(cross);
			if(!isnan(norm.x) && !isnan(norm.y) && !isnan(norm.z)){
				int insert = 1;
				for(size_t k = 0; k < cross_axes.busy; ++k)
					if(vec3_eq(cross_axes.data[k], norm))
					{ insert = 0; break; }
				if(insert)
					axes_list_push(&cross_axes, norm);
			}
		}

	float overlap = INFINITY;
	vec3f resolution;

	for(size_t k = 0; k < 3; ++k){
		axes_list* lst;
		switch(k){
			case 0: lst = &axes1; break;
			case 1: lst = &axes2; break;
			case 2: lst = &cross_axes; break;
		}
		for(size_t i = 0; i < lst->busy; ++i){
			vec3f axis = lst->data[i];
			if(vec3_ln(axis) == 0 || isnan(axis.x))
				continue;
			vec2f proj1 = body_project_on_axis(b1, axis),
			      proj2 = body_project_on_axis(b2, axis);
			if(do_proj_overlap(proj1, proj2)){
				float o = get_proj_overlap(proj1, proj2);
			
				// check for containment
				if(does_proj_contain(proj1, proj2) || does_proj_contain(proj2, proj1)){
					float mins = fabs(proj1.x - proj2.x);
					float maxs = fabs(proj1.y - proj2.y);
					o += min(mins, maxs);
				}

				// get the minimally overlapping axis
				if(o < overlap){
					overlap = o;
					resolution = axis;
				}
			}
			else
				return 0;
		}
	}
	resolution = vec3_smul(resolution, overlap);
	vec3f vd = vec3_sub(center1, center2);
	if(vec3_dot(vd, resolution) < 0)
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

				if(tpiece_bbox.max.y - bbox.min.y > 0.1 && bbox_check_collision(&bbox, &tpiece_bbox))
					return 1;
				piece = piece->next;
			}
		}
	return 0;
}

int sat_check_terrain_collision(body* b, vec3f* resolution, vec3f _forward, vec4f* new_rot)
{
	bbox3f bbox = body_get_bbox(b);
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
				body_hexahedron piece_body;
				body_init((body*)&piece_body, LUA_BODY_TYPE_HEXAHEDRON, sizeof(piece_body));
	
				piece_body.geom_cache.f[0].p[0] = (vec3f){tx, piece->z_floor[0], ty};
				piece_body.geom_cache.f[0].p[1] = (vec3f){tx + 1, piece->z_floor[1], ty};
				piece_body.geom_cache.f[0].p[2] = (vec3f){tx + 1, piece->z_floor[2], ty + 1};
				piece_body.geom_cache.f[0].p[3] = (vec3f){tx, piece->z_floor[3], ty + 1};
				piece_body.geom_cache.f[1].p[0] = (vec3f){tx, piece->z_floor[0], ty};
				piece_body.geom_cache.f[1].p[1] = (vec3f){tx + 1, piece->z_floor[1], ty};
				piece_body.geom_cache.f[1].p[2] = (vec3f){tx + 1, piece->z_ceil[1], ty};
				piece_body.geom_cache.f[1].p[3] = (vec3f){tx, piece->z_ceil[0], ty};
				piece_body.geom_cache.f[2].p[0] = (vec3f){tx, piece->z_floor[3], ty + 1};
				piece_body.geom_cache.f[2].p[1] = (vec3f){tx + 1, piece->z_floor[2], ty + 1};
				piece_body.geom_cache.f[2].p[2] = (vec3f){tx + 1, piece->z_ceil[2], ty + 1};
				piece_body.geom_cache.f[2].p[3] = (vec3f){tx, piece->z_ceil[3], ty + 1};
				piece_body.geom_cache.f[3].p[0] = (vec3f){tx, piece->z_floor[0], ty};
				piece_body.geom_cache.f[3].p[1] = (vec3f){tx, piece->z_floor[3], ty + 1};
				piece_body.geom_cache.f[3].p[2] = (vec3f){tx, piece->z_ceil[3], ty + 1};
				piece_body.geom_cache.f[3].p[3] = (vec3f){tx, piece->z_ceil[0], ty};
				piece_body.geom_cache.f[4].p[0] = (vec3f){tx + 1, piece->z_floor[1], ty};
				piece_body.geom_cache.f[4].p[1] = (vec3f){tx + 1, piece->z_floor[2], ty + 1};
				piece_body.geom_cache.f[4].p[2] = (vec3f){tx + 1, piece->z_ceil[2], ty + 1};
				piece_body.geom_cache.f[4].p[3] = (vec3f){tx + 1, piece->z_ceil[1], ty};
				piece_body.geom_cache.f[5].p[0] = (vec3f){tx, piece->z_ceil[0], ty};
				piece_body.geom_cache.f[5].p[1] = (vec3f){tx + 1, piece->z_ceil[1], ty};
				piece_body.geom_cache.f[5].p[2] = (vec3f){tx + 1, piece->z_ceil[2], ty + 1};
				piece_body.geom_cache.f[5].p[3] = (vec3f){tx, piece->z_ceil[3], ty + 1};

				for(size_t _f = 0; _f < 6; ++_f)
					for(size_t _p = 0; _p < 4; ++_p)
						piece_body.geom_cache.f[_f].p[_p] = vec3_smul(piece_body.geom_cache.f[_f].p[_p], TERRAIN_PIECE_SIZE);

				piece_body.geom = piece_body.geom_cache;

				vec3f resol;
				int _collided = sat_check_collision((body*)&piece_body, b, &resol);
				if(_collided){
					if(vec3_ln(resol) >= vec3_ln(max_resol)){
						max_resol = vec3_smul(resol, -1);
						vec3f e1 = vec3_sub(piece_body.geom_cache.f[5].p[0], piece_body.geom_cache.f[5].p[1]), e2 = vec3_sub(piece_body.geom_cache.f[5].p[1], piece_body.geom_cache.f[5].p[2]);
						vec3f norm = vec3_norm(vec3_cross(e1, e2));
						if(new_rot){
							mat4f trmat = mat4f_identity();
							vec3f up = vec3_smul(norm, -1);
							//printf("up: "); vec3f_print(up);
							vec3f forward = _forward;
							vec3f n = (vec3f){0, 1, 0};
							float forward_ang = rad_to_ang(acos(vec3_dot(n, up)));
							vec3f forward_axis = vec3_norm(vec3_cross(n, up));
							//printf("forward: "); vec3f_print(forward);
							mat4f forward_trmat = mat4f_identity();
							if(!isnan(forward_axis.x))
								mat4f_rotate(&forward_trmat, forward_ang, forward_axis);
							forward = vec3_norm(mat4f_mul_vec3f(&forward_trmat, forward));
							//printf("desired forward: "); vec3f_print(forward);
							vec3f right = vec3_norm(vec3_cross(forward, up));
							//printf("right: "); vec3f_print(right);

							trmat.e[0][0] = forward.x;
							trmat.e[0][1] = forward.y;
							trmat.e[0][2] = forward.z;
							trmat.e[1][0] = up.x;
							trmat.e[1][1] = up.y;
							trmat.e[1][2] = up.z;
							trmat.e[2][0] = right.x;
							trmat.e[2][1] = right.y;
							trmat.e[2][2] = right.z;

							//printf("trmat:\n"); mat4f_print(&trmat);
							*new_rot = quat_from_rot_mat(&trmat);
							//puts("");
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
