#include "math/box.h"

/* body */

vec3f box_get_vertice(vec3f _size, size_t idx)
{
	if(idx > 7) return (vec3f){NAN, NAN, NAN};
	return (vec3f){
		_size.x/2 * (idx & 1 ? 1 : -1),
		_size.y/2 * (idx & 2 ? 1 : -1),
		_size.z/2 * (idx & 4 ? 1 : -1)
	};
}
vec3f box_get_cached_vertice(vec3f* side_cache, vec3f pos, size_t idx)
{
	if(idx > 7) return (vec3f){NAN, NAN, NAN};
	vec3f v = {0, 0, 0};
	v = (idx & 1) ? vec3_add(v, side_cache[0]) : vec3_sub(v, side_cache[0]);
	v = (idx & 2) ? vec3_add(v, side_cache[1]) : vec3_sub(v, side_cache[1]);
	v = (idx & 4) ? vec3_add(v, side_cache[2]) : vec3_sub(v, side_cache[2]);
	v = vec3_add(v, pos);
	return v;
}
axes_list box_get_separating_axes_and_edges(vec3f* side_cache, size_t* normals_cnt)
{
	axes_list list;
	axes_list_create(&list);

	// get all face normals
	vec3f fn1 = vec3_norm(side_cache[0]), fn2 = vec3_norm(side_cache[1]), fn3 = vec3_norm(side_cache[2]);
	axes_list_push(&list, fn1);
	axes_list_push(&list, fn2);
	axes_list_push(&list, fn3);
	axes_list_push(&list, vec3_smul(fn1, -1));
	axes_list_push(&list, vec3_smul(fn2, -1));
	axes_list_push(&list, vec3_smul(fn3, -1));
	*normals_cnt = 6;

	// get all edges
	axes_list_push(&list, fn1);
	axes_list_push(&list, fn2);
	axes_list_push(&list, fn3);
	return list;
}
