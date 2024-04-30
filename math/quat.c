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
vec4f quat_from_rot(vec3f rot)
{
	rot.x = ang_to_rad(rot.x), rot.y = ang_to_rad(rot.y), rot.z = ang_to_rad(rot.z);
	float c1 = cos(rot.x/2), c2 = cos(rot.y/2), c3 = cos(rot.z/2),
	      s1 = sin(rot.x/2), s2 = sin(rot.y/2), s3 = sin(rot.z/2);
	return (vec4f){
		s1*s2*c3 + c1*c2*s3,
		s1*c2*c3 + c1*s2*s3,
		c1*s2*c3 - s1*c2*s3,
		c1*c2*c3 - s1*s2*s3
	};
}
vec3f rot_from_quat(vec4f q)
{
	return (vec3f){rad_to_ang(atan2(2*q.y*q.w - 2*q.x*q.z, 1 - 2*q.y*q.y - 2*q.z*q.z)),
			rad_to_ang(asin(2*q.x*q.y + 2*q.z*q.w)),
			rad_to_ang(atan2(2*q.x*q.w - 2*q.y*q.z, 1 - 2*q.x*q.x - 2*q.z*q.z))};
}

vec4f quat_from_rot_between(vec3f v1, vec3f v2)
{
	v1 = vec3_norm(v1), v2 = vec3_norm(v2);
	float ang = rad_to_ang(acos(vec3_dot(v1, v2)));
	vec3f axis = vec3_norm(vec3_cross(v1, v2));
	if(isnan(axis.x) || isnan(axis.y) || isnan(axis.z))
		return (vec4f){0, 0, 0, 1};
	else{
		mat4f trmat = mat4f_identity(); // get 3 separate rotation angles from rotation matrix (https://nghiaho.com/?page_id=846)
		mat4f_rotate(&trmat, ang, axis);
		return quat_from_rot_mat(&trmat);
	}
}
vec4f quat_slerp(vec4f start, vec4f end, float t)
{ // https://stackoverflow.com/questions/4099369/interpolate-between-rotation-matrices
	vec3f rot_start = rot_from_quat(start);
	vec3f rot_end = rot_from_quat(end);
	if(isnan(rot_start.x))
		return end;

	if(fabs(rot_start.x - rot_end.x) > 180){
		if(fabs(rot_start.x) > fabs(rot_end.x)) rot_start.x -= sign(rot_start.x)*360;
		else	rot_end.x -= sign(rot_end.x)*360;
	}
	if(fabs(rot_start.y - rot_end.y) > 180){
		if(fabs(rot_start.y) > fabs(rot_end.y)) rot_start.y -= sign(rot_start.y)*360;
		else	rot_end.y -= sign(rot_end.y)*360;
	}
	if(fabs(rot_start.z - rot_end.z) > 180){
		if(fabs(rot_start.z) > fabs(rot_end.z)) rot_start.z -= sign(rot_start.z)*360;
		else	rot_end.z -= sign(rot_end.z)*360;
	}

	rot_start = vec3_smul(rot_start, 1 - t);
	rot_end = vec3_smul(rot_end, t);
	vec3f rot_m = vec3_add(rot_start, rot_end);
	return quat_from_rot(rot_m);
}
