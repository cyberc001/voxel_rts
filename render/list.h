#ifndef RENDER_LIST_H
#define RENDER_LIST_H

#include "render/base.h"
#include "math/vec.h"
#include "math/quat.h"

typedef struct {
	const render_obj* robj;
	vec3f tr;
	vec4f rot;
	vec3f sc;
} render_info;

typedef struct {
	size_t ln;
	render_info* data;
} render_list;
extern render_list *render_list_front, *render_list_mid, *render_list_back;

void render_list_init();
void render_list_add(const render_info* inf);

void render_list_swap_back_and_mid();
void render_list_swap_front_and_mid();

#endif
