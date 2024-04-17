#include "math/quat.h"

vec4f quat_from_rot_mat(mat4f* m)
{ // https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
	float trace = mat4f_trace(*m) - m->e[3][3];
	if(trace > 0){
		float s = 0.5 / sqrtf(trace + 1);
		return (vec4f){
			(m->e[2][1] - m->e[1][2])*s,
			(m->e[0][2] - m->e[2][0])*s,
			(m->e[1][0] - m->e[0][1])*s,
			0.25/s
		};
	}
	if(m->e[0][0] > m->e[1][1] && m->e[0][0] > m->e[2][2]){
		float s = 2*sqrtf(1 + m->e[0][0] - m->e[1][1] - m->e[2][2]);
		return (vec4f){
			0.25*s,
			(m->e[0][1] + m->e[1][0])/s,
			(m->e[0][2] + m->e[2][0])/s,
			(m->e[2][1] - m->e[1][2])/s
		};
	}
	if(m->e[1][1] > m->e[2][2]){
		float s = 2*sqrtf(1 + m->e[1][1] - m->e[0][0] - m->e[2][2]);
		return (vec4f){
			(m->e[0][1] + m->e[1][0])/s,
			0.25*s,
			(m->e[1][2] + m->e[2][1])/s,
			(m->e[0][2] - m->e[2][0])/s
		};
	}
	float s = 2*sqrtf(1 + m->e[2][2] - m->e[0][0] - m->e[1][1]);
	return (vec4f){
		(m->e[0][2] + m->e[2][0])/s,
		(m->e[1][2] + m->e[2][1])/s,
		0.25*s,
		(m->e[1][0] - m->e[0][1])/s
	};
}
mat4f mat_from_quat(vec4f q)
{
	return (mat4f){{
		{1 - 2*q.y*q.y - 2*q.z*q.z, 2*q.x*q.y - 2*q.z*q.w, 2*q.x*q.z + 2*q.y*q.w, 0},
		{2*q.x*q.y + 2*q.z*q.w, 1 - 2*q.x*q.x - 2*q.z*q.z, 2*q.y*q.z - 2*q.x*q.w, 0},
		{2*q.x*q.z - 2*q.y*q.w, 2*q.y*q.z + 2*q.x*q.w, 1 - 2*q.x*q.x - 2*q.y*q.y, 0},
		{0, 0, 0, 1}
	}};
}

vec4f quat_slerp(vec4f start, vec4f end, float t)
{ // https://stackoverflow.com/questions/4099369/interpolate-between-rotation-matrices
	float cos_half_theta = vec4_dot(start, end);
	if(fabs(cos_half_theta) >= 1)
		return start;
	float half_theta = acos(cos_half_theta);
	float sin_half_theta = sqrt(1 - cos_half_theta*cos_half_theta);
	if(fabs(sin_half_theta) < 0.001){
		vec4f m1 = vec4_smul(start, 0.5), m2 = vec4_smul(end, 0.5);
		return vec4_add(m1, m2);
	}
	float ratio_a = sin((1 - t) * half_theta) / sin_half_theta;
	float ratio_b = sin(t * half_theta) / sin_half_theta;
	vec4f m1 = vec4_smul(start, ratio_a), m2 = vec4_smul(end, ratio_b);
	return vec4_add(m1, m2);
}
