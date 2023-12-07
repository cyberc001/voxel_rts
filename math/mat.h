#ifndef MATH_MAT_H
#define MATH_MAT_H

typedef struct {
	float e[4][4]; // [row][column]
} mat4f;


void mat4f_print(const mat4f* mat);


#define mat4f_identity() (mat4f){.e = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}}

#endif
