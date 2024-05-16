#ifndef RENDER_LIST_H
#define RENDER_LIST_H

#include "render/base.h"
#include "math/vec.h"
#include "math/quat.h"
#include "dyn_array.h"

#define RENDER_INFO_TYPE_DRAW		1
#define RENDER_INFO_TYPE_FREE		1

typedef struct {
	render_obj* robj;
	vec3f tr;
	vec4f rot;
	vec3f sc;
} render_info_draw;
typedef struct {
	GLuint buf;
	void* attr_data[RENDER_OBJ_ATTRIBUTES_COUNT];
} render_info_free;

DEF_DYN_ARRAY(render_info_draw_dynarray, render_info_draw)
DEF_DYN_ARRAY(render_info_free_dynarray, render_info_free)
typedef struct {
	render_info_draw_dynarray draw;
	render_info_free_dynarray free;
} render_list;
extern render_list *render_list_front, *render_list_mid, *render_list_back;

void render_list_init(); // initialize front/mid/back lists
render_list* render_list_create(); // uses malloc()
void render_list_reset(render_list* list); // set all list lengths to 0

// add things to back list
void render_list_add_draw(render_info_draw* inf);
void render_list_add_free(render_info_free* inf);

void render_list_swap_back_and_mid();
void render_list_swap_front_and_mid();

#endif
