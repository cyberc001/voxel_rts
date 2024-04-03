#ifndef MATH_MAT_H
#define MATH_MAT_H

#include "math/vec.h"

typedef struct {
	float e[4][4]; // [row][column]
} mat4f;

void mat4f_print(const mat4f* mat);
#define mat4f_gl_print(mat_gl) { mat4f __gl_mat = mat4f_from_gl(mat_gl); mat4f_print(&__gl_mat); }

#define mat4f_identity() (mat4f){.e = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}}
#define mat4f_from_gl(mat_gl) (mat4f){.e = {{(mat_gl)[0], (mat_gl)[4], (mat_gl)[8], (mat_gl)[12]}, {(mat_gl)[1], (mat_gl)[5], (mat_gl)[9], (mat_gl)[13]}, {(mat_gl)[2], (mat_gl)[6], (mat_gl)[10], (mat_gl)[14]}, {(mat_gl)[3], (mat_gl)[7], (mat_gl)[11], (mat_gl)[15]}}}

mat4f mat4f_mul(mat4f* m1, const mat4f* m2);
vec3f mat4f_mul_vec3f(mat4f* m, vec3f v); // in vec3f w = 1

void mat4f_translate(mat4f* mat, vec3f tr);
void mat4f_rotate(mat4f* mat, float angle, vec3f rot); // normalizes vec3f rot just like OpenGL does
void mat4f_scale(mat4f* mat, vec3f sc);

vec2f vec3f_project2(vec3f vec);

#endif
