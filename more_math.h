#ifndef MORE_MATH_H
#define MORE_MATH_H

#include <math.h>
#include "math/vec.h"
#include "math/mat.h"

#define swap(x, y) {typeof(x) __tmp = (x); (x) = (y); (y) = __tmp; }

typedef struct {
	vec3f p1, p2;
} line3f;

typedef struct {
	vec2f p[4];
} face2f;

typedef struct {
	vec3f min, max;
} bbox3f;

#define min(x, y) ((x) > (y) ? (y) : (x))
#define max(x, y) ((x) > (y) ? (x) : (y))
#define clamp(x, _min, _max) (min(max((x), (_min)), (_max)))
#define clamp01(x, _min, _max) ( ( (double)clamp(x, _min, _max) - (_min) ) / ((_max) - (_min)) )

#define sign(x) ((x) < 0 ? -1 : !!(x))
#define frac(x) ((x) - floor(x))

#define EQF_PRECISION	1e-6
#define eqf(a, b) (fabs((a) - (b)) <= EQF_PRECISION)

// trigonometry
float clamp_angle_360(float a);
#define ang_to_rad(a) ((a) / 180. * M_PI)
#define rad_to_ang(a) ((a) * 180. / M_PI)

// collision checks
#define in_range(x, _min, _max) ((x) >= (_min) && (x) <= (_max))
int is_point_in_rect2f(vec2f point, rect2f rect);

// misc
vec3f perpendicular_3f(line3f line, vec3f point);

// Translates 2D ortho coordinates from (0; 1) to (-1; 1). I couldn't change them with gluOrtho for some reason, they are always stuck at (-1; 1).
#define TRANSLATE_ORTHO_COORDS(v) {(v).x = (v).x*2 - 1; (v).y = (v).y*2 - 1;}
#define TRANSLATE_ORTHO_SIZE(v) {(v).x *= 2; (v).y *= 2;}

#endif
