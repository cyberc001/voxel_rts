#ifndef MATH_VEC_H
#define MATH_VEC_H

// Function notations:
// s - scalar

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
	float x, y, z, w;
} vec4f;

typedef struct {
	vec2f pos, size;
} rect2f;

void vec2i_print(vec2i vec);

void vec2f_print(vec2f vec);
void vec3f_print(vec3f vec);
void vec4f_print(vec4f vec);

#define vec3_eq(v1, v2) ( (v1).x == (v2).x && (v1).y == (v2).y && (v1).z == (v2).z )
#define vec2_eq(v1, v2) ( (v1).x == (v2).x && (v1).y == (v2).y )

#define vec3_smul(vec, s) ( (typeof(vec)){(vec).x * (s), (vec).y * (s), (vec).z * (s)} )
#define vec4_smul(vec, s) ( (typeof(vec)){(vec).x * (s), (vec).y * (s), (vec).z * (s), (vec).w * (s)} )
#define vec3_sdiv(vec, s) ( (typeof(vec)){(vec).x / (s), (vec).y / (s), (vec).z / (s)} )
#define vec4_sdiv(vec, s) ( (typeof(vec)){(vec).x / (s), (vec).y / (s), (vec).z / (s), (vec).w / (s)} )

#define vec2_add(v1, v2) ( (typeof(v1)){(v1).x + (v2).x, (v1).y + (v2).y} )
#define vec3_add(v1, v2) ( (typeof(v1)){(v1).x + (v2).x, (v1).y + (v2).y, (v1).z + (v2).z} )
#define vec4_add(v1, v2) ( (typeof(v1)){(v1).x + (v2).x, (v1).y + (v2).y, (v1).z + (v2).z, (v1).w + (v2).w} )

#define vec2_sub(v1, v2) ( (typeof(v1)){(v1).x - (v2).x, (v1).y - (v2).y} )
#define vec3_sub(v1, v2) ( (typeof(v1)){(v1).x - (v2).x, (v1).y - (v2).y, (v1).z - (v2).z} )
#define vec4_sub(v1, v2) ( (typeof(v1)){(v1).x - (v2).x, (v1).y - (v2).y, (v1).z - (v2).z, (v1).w - (v2).w} )

#define vec2_ln(vec) ( sqrt((vec).x*(vec).x + (vec).y*(vec).y) )
#define vec3_ln(vec) ( sqrt((vec).x*(vec).x + (vec).y*(vec).y + (vec).z*(vec).z) )
#define vec4_ln(vec) ( sqrt((vec).x*(vec).x + (vec).y*(vec).y + (vec).z*(vec).z + (vec).w*(vec).w) )
#define vec3_ln_safe(vec) (((vec).x == 0 && (vec).y == 0 && (vec).z == 0) ? 0 : vec3_ln(vec))
#define vec3_norm(vec) vec3_sdiv(vec, vec3_ln(vec))
#define vec4_norm(vec) vec4_sdiv(vec, vec4_ln(vec))

#define vec3_cross(v1, v2) ( (typeof(v1)){(v1).y*(v2).z - (v1).z*(v2).y, (v1).z*(v2).x - (v1).x*(v2).z, (v1).x*(v2).y - (v1).y*(v2).x})
#define vec2_dot(v1, v2) ( (v1).x*(v2).x + (v1).y*(v2).y )
#define vec3_dot(v1, v2) ( (v1).x*(v2).x + (v1).y*(v2).y + (v1).z*(v2).z )
#define vec4_dot(v1, v2) ( (v1).x*(v2).x + (v1).y*(v2).y + (v1).z*(v2).z + (v1).w*(v2).w )

// rotation
vec3f vec3f_rot(vec3f v, vec3f rot); // rotate a vector using transformation matrix
float vec3f_get_ang_between(vec3f v1, vec3f v2);
vec3f vec3f_get_rot_between(vec3f v1, vec3f v2); // returns a rotation vector between v1 and v2. Returns (NAN, NAN, NAN) if rotation is not needed.

// misc
#define vec3_setmin(v1, vmin) { if((v1).x < (vmin).x) (v1).x = (vmin).x; if((v1).y < (vmin).y) (v1).y = (vmin).y; if((v1).z < (vmin).z) (v1).z = (vmin).z; }

#endif
