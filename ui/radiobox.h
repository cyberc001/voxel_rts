#ifndef RADIOBOX_H
#define RADIOBOX_H

#include "dyn_array.h"
#include "ui/ui_element.h"
#include "render/font.h"
#include "render/texture.h"

typedef struct radiobox radiobox;
DEF_DYN_ARRAY(radiobox_group_arr, radiobox*)
typedef struct {
	radiobox_group_arr arr;
	radiobox* selection;
} radiobox_group;

void radiobox_group_create(radiobox_group* rg);
size_t radiobox_group_push(radiobox_group* rg, radiobox* rb);
void radiobox_group_delete(radiobox_group* rg, size_t i);
void radiobox_group_delete_last(radiobox_group* rg);
void radiobox_group_select(radiobox_group* rg, radiobox* rb);
void radiobox_group_free(radiobox_group* rg);

struct radiobox {
	struct ui_element;

	vec2f font_size; vec3f text_clr; font* fnt;
	const char* text;

	atlas_texture *check_tex, *uncheck_tex;
	render_obj ro_check, ro_uncheck, ro_text;

	radiobox_group* group;
	int checked;
};

void radiobox_create(radiobox* o, vec2f post, vec2f size, vec2f font_size,
			font* fnt, const char* text,
			atlas_texture* check_tex, atlas_texture* uncheck_tex);

void radiobox_set_group(radiobox* o, radiobox_group* rg);

#endif
