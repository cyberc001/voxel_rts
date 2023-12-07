#include "more_math.h"

float clamp_angle_360(float a)
{
	a = fmodf(a, 360);
	if(a < 0) a = 360 + a;
	return a;
}

int is_point_in_rect2f(vec2f point, vec2f coords, vec2f size)
{
	return point.x >= coords.x && point.x <= coords.x + size.x
		&& point.y >= coords.y && point.y <= coords.y + size.y;
}
