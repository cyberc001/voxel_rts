#ifndef MORE_MATH_H
#define MORE_MATH_H

#include <math.h>
#include "math/vec.h"
#include "math/mat.h"

typedef struct {
	vec3f p1, p2;
} line3f;

#define min(x, y) ((x) > (y) ? (y) : (x))
#define max(x, y) ((x) > (y) ? (x) : (y))
#define clamp(x, _min, _max) (min(max((x), (_min)), (_max)))
#define clamp01(x, _min, _max) ( ( (double)clamp(x, _min, _max) - (_min) ) / ((_max) - (_min)) )

#define sign(x) ((x) < 0 ? -1 : !!(x))

// trigonometry
float clamp_angle_360(float a);
#define ang_to_rad(a) ((a) / 180. * M_PI)
#define rad_to_ang(a) ((a) * 180. / M_PI)

// collision checks
int is_point_in_rect2f(vec2f point, vec2f coords, vec2f size);

// misc
vec3f perpendicular_3f(line3f line, vec3f point);

// Translates 2D ortho coordinates from (0; 1) to (-1; 1). I couldn't change them with gluOrtho for some reason, they are always stuck at (-1; 1).
#define TRANSLATE_ORTHO_COORDS(v) {(v).x = (v).x*2 - 1; (v).y = (v).y*2 - 1;}
#define TRANSLATE_ORTHO_SIZE(v) {(v).x *= 2; (v).y *= 2;}

#endif
