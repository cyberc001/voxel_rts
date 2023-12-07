#include "math/mat.h"
#include <stdio.h>

void mat4f_print(const mat4f* mat)
{
	for(size_t i = 0; i < 4; ++i){
		for(size_t j = 0; j < 4; ++j)
			printf("%f ", mat->e[i][j]);
		puts("");
	}
}


void mat4f_translate(mat4f* mat, vec3f tr)
{

}
