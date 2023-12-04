#ifndef MORE_MATH_H
#define MORE_MATH_H

#include <math.h>

#define min(x, y) ((x) > (y) ? (y) : (x))
#define max(x, y) ((x) > (y) ? (x) : (y))
#define clamp(x, _min, _max) (min(max((x), (_min)), (_max)))
#define clamp01(x, _min, _max) ( ( (double)clamp(x, _min, _max) - (_min) ) / ((_max) - (_min)) )

typedef struct {
	int x, y;
} vec2i;

typedef struct {
	float x, y;
} vec2f;
typedef struct {
	float x, y, z;
} vec3f;

typedef struct {
	vec2f pos, size;
} rect2f;

#define vec3_smul(vec, s) ( (typeof(vec)){(vec).x * s, (vec).y * s, (vec).z * s} )

#define vec2_add(v1, v2) ( (typeof(v1)){(v1).x + (v2).x, (v1).y + (v2).y} )
#define vec3_add(v1, v2) ( (typeof(v1)){(v1).x + (v2).x, (v1).y + (v2).y, (v1).z + (v2).z} )

#define vec2_sub(v1, v2) ( (typeof(v1)){(v1).x - (v2).x, (v1).y - (v2).y} )

// trigonometry
float clamp_angle_360(float a);
#define ang_to_rad(a) ((a) / 180. * M_PI)

// collision checks
int is_point_in_rect2f(vec2f point, vec2f coords, vec2f size);

// Translates 2D ortho coordinates from (0; 1) to (-1; 1). I couldn't change them with gluOrtho for some reason, they are always stuck at (-1; 1).
#define TRANSLATE_ORTHO_COORDS(v) {(v).x = (v).x*2 - 1; (v).y = (v).y*2 - 1;}
#define TRANSLATE_ORTHO_SIZE(v) {(v).x *= 2; (v).y *= 2;}

#endif
