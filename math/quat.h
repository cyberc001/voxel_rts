#ifndef QUAT_H
#define QUAT_H

#include "more_math.h"
#include "math/vec.h"
#include "math/mat.h"

vec4f quat_from_rot_mat(mat4f* m);
mat4f mat_from_quat(vec4f q);

vec4f quat_slerp(vec4f start, vec4f end, float t);

#endif
