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

vec3f perpendicular_3f(line3f line, vec3f point)
{ // source: https://math.stackexchange.com/questions/4347497/find-a-point-on-a-line-that-creates-a-perpendicular-in-3d-space
	vec3f sub1 = vec3_sub(point, line.p1), sub2 = vec3_sub(line.p2, line.p1);
	float t = vec3_dot(sub1, sub2) / vec3_dot(sub2, sub2);
	return vec3_add(line.p1, vec3_smul(sub2, t));
}
