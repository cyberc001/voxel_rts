#include "math/vec.h"
#include <stdio.h>

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
