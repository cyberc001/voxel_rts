#include "math/mat.h"
#include "more_math.h"
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>

void mat4f_print(const mat4f* mat)
{
	for(size_t i = 0; i < 4; ++i){
		for(size_t j = 0; j < 4; ++j)
			printf("%f ", mat->e[i][j]);
		puts("");
	}
}

mat4f mat4f_mul(mat4f* m1, const mat4f* m2)
{
	mat4f m;
	for(size_t i = 0; i < 4; ++i) // i -> row in 1st matrix (row in result)
	for(size_t j = 0; j < 4; ++j){ // j -> column in 2nd matrix
		m.e[i][j] = 0;
		for(size_t k = 0; k < 4; ++k) // k -> column in 1st matrix (row in 2nd)
			m.e[i][j] += m1->e[i][k] * m2->e[k][j];
	}
	return m;
}
vec3f mat4f_mul_vec3f(mat4f* m, vec3f v) // in vec3f w = 1
{
	return (vec3f){m->e[0][0]*v.x + m->e[0][1]*v.y + m->e[0][2]*v.z + m->e[0][3],
			m->e[1][0]*v.x + m->e[1][1]*v.y + m->e[1][2]*v.z + m->e[1][3],
			m->e[2][0]*v.x + m->e[2][1]*v.y + m->e[2][2]*v.z + m->e[2][3]};
}
vec4f mat4f_mul_vec4f(mat4f* m, vec4f v)
{
	return (vec4f){m->e[0][0]*v.x + m->e[0][1]*v.y + m->e[0][2]*v.z + m->e[0][3]*v.w,
			m->e[1][0]*v.x + m->e[1][1]*v.y + m->e[1][2]*v.z + m->e[1][3]*v.w,
			m->e[2][0]*v.x + m->e[2][1]*v.y + m->e[2][2]*v.z + m->e[2][3]*v.w,
			m->e[3][0]*v.x + m->e[3][1]*v.y + m->e[3][2]*v.z + m->e[3][3]*v.w};
}


void mat4f_translate(mat4f* mat, vec3f tr)
{
	mat4f tr_mat = {{{1, 0, 0, tr.x},
			{0, 1, 0, tr.y},
			{0, 0, 1, tr.z},
			{0, 0, 0, 1}}};
	*mat = mat4f_mul(mat, &tr_mat);
}
void mat4f_rotate(mat4f* mat, float angle, vec3f rot)
{
	float a = ang_to_rad(angle);
	float c = cosf(a), s = sinf(a);
	if(vec3_ln(rot) > 0) rot = vec3_norm(rot);
	mat4f rot_mat = {{{rot.x*rot.x*(1-c)+c, rot.x*rot.y*(1-c) - rot.z*s, rot.x*rot.z*(1-c)+rot.y*s, 0},
			 {rot.y*rot.x*(1-c)+rot.z*s, rot.y*rot.y*(1-c)+c, rot.y*rot.z*(1-c)-rot.x*s, 0},
			 {rot.z*rot.x*(1-c)-rot.y*s, rot.z*rot.y*(1-c)+rot.x*s, rot.z*rot.z*(1-c)+c, 0},
			 {0, 0, 0, 1}
	}};
	*mat = mat4f_mul(mat, &rot_mat);
}

void mat4f_scale(mat4f* mat, vec3f sc)
{
	mat4f sc_mat = {{{sc.x, 0, 0, 0},
			{0, sc.y, 0, 0},
			{0, 0, sc.z, 0},
			{0, 0, 0, 1}}};
	*mat = mat4f_mul(mat, &sc_mat);
}

vec2f vec3f_project(vec3f v)
{
	GLfloat modelview_mat[16];
	GLfloat projection_mat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_mat);
	glGetFloatv(GL_PROJECTION_MATRIX, projection_mat);
	mat4f model_mat4 = mat4f_from_gl(modelview_mat);
	mat4f proj_mat4 = mat4f_from_gl(projection_mat);

	vec4f vec = (vec4f){v.x, v.y, v.z, 1};
	vec = mat4f_mul_vec4f(&model_mat4, vec);
	vec = mat4f_mul_vec4f(&proj_mat4, vec);
	vec = vec4_sdiv(vec, vec.w);
	return (vec2f){vec.x, vec.y};
}
