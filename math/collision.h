#ifndef MATH_COLLISION_H
#define MATH_COLLISION_H

#include "math/vec.h"
#include "game/terrain.h"
#include "math/hexahedron.h"
#include "math/body.h"

float triangle3_area_heron(vec3f a, vec3f b, vec3f c); // a, b, c are points, not edge vectors
float triangle2_area_heron(vec2f a, vec2f b, vec2f c); // a, b, c are points, not edge vectors

float line_plane_intersect(line3f line, vec3f surf_norm, vec3f surf_p, vec3f* intersect, vec3f* collided_point);
int point_is_in_face2(vec2f pt, const face2f* f);
int point_is_in_face3(vec3f pt, const face3f* f);

int hexahedron_check_projection_collision(const hexahedron* h1, const hexahedron* h2);
int bbox_check_collision(const bbox3f* b1, const bbox3f* b2);
int bbox_contains_bbox(const bbox3f* container, const bbox3f* containee);

int bbox_check_terrain_collision(bbox3f bbox);

int sat_check_collision(body* b1, body* b2, vec3f* resolution);
int sat_check_terrain_collision(body* b, vec3f* resolution, vec3f _forward, vec4f* new_rot);

#endif
