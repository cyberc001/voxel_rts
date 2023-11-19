#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "ui/ui_element.h"
#include "render/font.h"
#include "render/texture.h"
#include <pthread.h>

typedef struct textbox textbox;
struct textbox {
	struct ui_element;

	vec2f font_size;
	font* fnt;

	char* text; size_t text_ln;
	float text_w; float space_w;

	vec3f text_clr;
	struct ui_lrm_box_textures box_tex;
	atlas_texture *left, *right, *mid;

	render_obj ro_text, ro_cursor, ro_atex;
};

void textbox_create(textbox* o, vec2f pos, vec2f size, vec2f font_size,
			struct ui_lrm_box_textures box_tex,
			font* fnt);

#endif
