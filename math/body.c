#include "math/body.h"
#include "math/collision.h"
#include "math/quat.h"

#include "math/hexahedron.h"
#include "math/box.h"
#include "render/primitive.h"

#include "controls/selection.h"

void body_init(body* b, int type, size_t size)
{
	b->type = type;
	b->size = size;

	b->transform.pos = (vec3f){0, 0, 0};
	b->transform.rot = (vec4f){0, 0, 0, 1};
	b->transform.scale = (vec3f){1, 1, 1};
	b->transform.dirty = 1;

	body_generate_cache(b);
}

void body_generate_cache(body* b)
{
	if(!b->transform.dirty)
		return;
	b->transform.dirty = 0;
	switch(b->type){
		case LUA_BODY_TYPE_BOX: {
			body_box* bb = (body_box*)b;
			bb->side_cache[0] = (vec3f){bb->_size.x/2, 0, 0};
			bb->side_cache[1] = (vec3f){0, bb->_size.y/2, 0};
			bb->side_cache[2] = (vec3f){0, 0, bb->_size.z/2};

			vec3f pos = b->transform.pos;
			b->transform.pos = (vec3f){0, 0, 0};
			for(size_t i = 0; i < 3; ++i)
				bb->side_cache[i] = vec3_transform_by_body(b, bb->side_cache[i]);
			b->transform.pos = pos;
		} break;
		case LUA_BODY_TYPE_HEXAHEDRON: {
			body_hexahedron* bh = (body_hexahedron*)b;
			bh->geom_cache = bh->geom;

			mat4f rot_mat = mat_from_quat(b->transform.rot);
			for(size_t i = 0; i < 6; ++i)
				for(size_t j = 0; j < 4; ++j){
					vec4f v = vec4f_mul_mat4f(&rot_mat, (vec4f){bh->geom_cache.f[i].p[j].x, bh->geom_cache.f[i].p[j].y, bh->geom_cache.f[i].p[j].z, 1});
					bh->geom_cache.f[i].p[j] = (vec3f){v.x, v.y, v.z};
				}

			mat4f trmat = mat4f_identity();
			mat4f_translate(&trmat, b->transform.pos);
			mat4f_scale(&trmat, b->transform.scale);
		
			bh->geom_cache = hexahedron_transform(&bh->geom_cache, &trmat);
		} break;
	}
}
vec3f body_get_vertice(const body* b, size_t idx)
{
	switch(b->type){
		case LUA_BODY_TYPE_BOX:
			return box_get_vertice(((body_box*)b)->_size, idx);
		case LUA_BODY_TYPE_HEXAHEDRON:
			return hexahedron_get_vertice(&((body_hexahedron*)b)->geom, idx);
	}
	return (vec3f){NAN, NAN, NAN};
}
vec3f body_get_cached_vertice(const body* b, size_t idx)
{
	switch(b->type){
		case LUA_BODY_TYPE_BOX:
			return box_get_cached_vertice(((body_box*)b)->side_cache, b->transform.pos, idx);
		case LUA_BODY_TYPE_HEXAHEDRON:
			return hexahedron_get_vertice(&((body_hexahedron*)b)->geom_cache, idx);
	}
	return (vec3f){NAN, NAN, NAN};
}

render_obj body_render(const body* b)
{
	switch(b->type){
		case LUA_BODY_TYPE_BOX:
			return generate_box(((body_box*)b)->_size);
		case LUA_BODY_TYPE_HEXAHEDRON:
			return generate_hexahedron(&((body_hexahedron*)b)->geom);
	}
	return RENDER_OBJ_EMPTY;
}

vec3f body_get_center(const body* b)
{
	vec3f center = {0, 0, 0};
	size_t idx = 0; vec3f p;
	while(p = body_get_vertice(b, idx++), !isnan(p.x))
		center = vec3_add(center, p);
	center = vec3_sdiv(center, idx - 1);
	center = vec3_transform_by_body(b, center);
	return center;
}
#define SET_MIN(where, what) {if((what) < (where)) (where) = (what);}
#define SET_MAX(where, what) {if((what) > (where)) (where) = (what);}
bbox3f body_get_base_bbox(body* b)
{
	bbox3f bbox = {
		.min = {INFINITY, INFINITY, INFINITY},
		.max = {-INFINITY, -INFINITY, -INFINITY}
	};
	size_t idx = 0; vec3f p;
	while(p = body_get_vertice(b, idx++), !isnan(p.x)){
		SET_MIN(bbox.min.x, p.x);
		SET_MIN(bbox.min.y, p.y);
		SET_MIN(bbox.min.z, p.z);
		SET_MAX(bbox.max.x, p.x);
		SET_MAX(bbox.max.y, p.y);
		SET_MAX(bbox.max.z, p.z);
	}
	return bbox;
}
bbox3f body_get_bbox(body* b)
{
	body_generate_cache(b);
	bbox3f bbox = {
		.min = {INFINITY, INFINITY, INFINITY},
		.max = {-INFINITY, -INFINITY, -INFINITY}
	};
	size_t idx = 0; vec3f p;
	while(p = body_get_cached_vertice(b, idx++), !isnan(p.x)){
		SET_MIN(bbox.min.x, p.x);
		SET_MIN(bbox.min.y, p.y);
		SET_MIN(bbox.min.z, p.z);
		SET_MAX(bbox.max.x, p.x);
		SET_MAX(bbox.max.y, p.y);
		SET_MAX(bbox.max.z, p.z);
	}
	return bbox;
}

vec3f vec3_transform_by_body(const body* b, vec3f v)
{
	mat4f rot_mat = mat_from_quat(b->transform.rot);
	vec4f v_rotated = vec4f_mul_mat4f(&rot_mat, (vec4f){v.x, v.y, v.z, 1});
	v = (vec3f){v_rotated.x, v_rotated.y, v_rotated.z};
	mat4f trmat = mat4f_identity();
	mat4f_translate(&trmat, b->transform.pos);
	mat4f_scale(&trmat, b->transform.scale);
	v = mat4f_mul_vec3f(&trmat, v);
	return v;
}

axes_list body_get_separating_axes_and_edges(body* b, size_t* normals_cnt)
{
	body_generate_cache(b);
	switch(b->type){
		case LUA_BODY_TYPE_BOX:
			return box_get_separating_axes_and_edges(((body_box*)b)->side_cache, normals_cnt);
		case LUA_BODY_TYPE_HEXAHEDRON:
			return hexahedron_get_separating_axes_and_edges(&((body_hexahedron*)b)->geom_cache, normals_cnt);
	}

	*normals_cnt = 0;
	axes_list list;
	axes_list_create(&list);
	return list;
}
vec2f body_project_on_axis(body* b, vec3f axis)
{
	body_generate_cache(b);

	float _min = INFINITY, _max = -INFINITY;
	size_t idx = 0; vec3f p;
	while(p = body_get_cached_vertice(b, idx++), !isnan(p.x)){
		float proj = vec3_dot(axis, p);
		if(proj < _min) _min = proj;
		if(proj > _max) _max = proj;
	}
	return (vec2f){_min, _max};
}
int body_is_selected(body* b)
{
	bbox3f bbox = body_get_bbox(b);
	vec2f proj_min = {INFINITY, INFINITY}, proj_max = {-INFINITY, -INFINITY};
	for(int max_bits = 0; max_bits < 8; ++max_bits){
		vec3f pt = {
			max_bits & 1 ? bbox.max.x : bbox.min.x,
			max_bits & 2 ? bbox.max.y : bbox.min.y,
			max_bits & 4 ? bbox.max.z : bbox.min.z,
		};
		vec2f proj = vec3f_project2(pt);
		SET_MIN(proj_min.x, proj.x);
		SET_MIN(proj_min.y, proj.y);
		SET_MAX(proj_max.x, proj.x);
		SET_MAX(proj_max.y, proj.y);
	}

	return proj_min.x < controls_selection_max.x && proj_max.x > controls_selection_min.x && proj_min.y < controls_selection_max.y && proj_max.y > controls_selection_min.y;
}
