#ifndef MATH_BODY_H
#define MATH_BODY_H

#include <stddef.h>
#include "math/vec.h"
#include "more_math.h"
#include "render/base.h"

typedef struct body body;
struct body {
	int type;
	size_t size;

	struct {
		vec3f pos;
		vec4f rot;
		vec3f scale;
		int dirty; // if 1, geom_cache should be regenerated before doing any operations
	} transform;
};

#define LUA_BODY_TYPE_BOX		1
#define LUA_BODY_TYPE_HEXAHEDRON	2

void body_init(body* b, int type, size_t size);

void body_generate_cache(body* b);
vec3f body_get_vertice(const body* b, size_t idx);
vec3f body_get_cached_vertice(const body* b, size_t idx);

render_obj body_render(const body* b);

vec3f body_get_center(const body* b);
bbox3f body_get_bbox(body* b);

vec3f vec3_transform_by_body(const body* b, vec3f v);

#include "dyn_array.h"
DEF_DYN_ARRAY(axes_list, vec3f)

axes_list body_get_separating_axes_and_edges(body* b, size_t* normals_cnt);
vec2f body_project_on_axis(body* b, vec3f axis);
int body_is_selected(body* b);

#endif
