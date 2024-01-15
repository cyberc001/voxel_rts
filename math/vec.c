#include "more_math.h"
#include "math/vec.h"
#include "math/mat.h"
#include <stdio.h>

// debug
void vec2f_print(vec2f vec)
{
	printf("{%f, %f}\n", vec.x, vec.y);
}
void vec2i_print(vec2i vec)
{
	printf("{%d, %d}\n", vec.x, vec.y);
}
void vec3f_print(vec3f vec)
{
	printf("{%f, %f, %f}\n", vec.x, vec.y, vec.z);
}

// rotation
vec3f vec3f_rot(vec3f v, vec3f rot)
{
	mat4f trmat = mat4f_identity();
	mat4f_rotate(&trmat, rot.x, (vec3f){1, 0, 0});
	mat4f_rotate(&trmat, rot.y, (vec3f){0, 1, 0});
	mat4f_rotate(&trmat, rot.z, (vec3f){0, 0, 1});
	return mat4f_mul_vec3f(&trmat, v);
}
vec3f vec3f_get_rot_between(vec3f v1, vec3f v2)
{ // source: https://stackoverflow.com/questions/42554960/get-xyz-angles-between-vectors
	float ang = rad_to_ang(acos(vec3_dot(v1, v2) / vec3_ln(v1) / vec3_ln(v2)));
	vec3f axis = vec3_norm(vec3_cross(v1, v2));
	if(isnan(axis.x) || isnan(axis.y) || isnan(axis.z))
		return (vec3f){NAN, NAN, NAN};
	else
		return vec3_smul(axis, ang);
}	
vec3f vec3f_lookat_rot(vec3f cur_rot, vec3f look_pt)
{
	vec3f v = vec3f_rot((vec3f){1, 0, 0}, cur_rot);
	return vec3f_get_rot_between(v, look_pt);
}
