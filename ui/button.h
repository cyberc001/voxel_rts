#ifndef BUTTON_H
#define BUTTON_H

#include "ui/ui_element.h"
#include "render/font.h"
#include "render/texture.h"

typedef struct button button;
struct button {
	struct ui_element;

	vec2f font_size;
	font* fnt;
	const char* text;
	vec3f text_clr, text_clr_p;
	struct ui_lrm_box_textures box_tex, box_tex_p;

	render_obj ro_text, ro_text_p,
		   ro_atex, ro_atex_p;

	int pressed;
};

void button_create(button* o, vec2f pos, vec2f size, vec2f font_size,
			struct ui_lrm_box_textures box_tex, struct ui_lrm_box_textures box_tex_p,
			font* fnt, const char* text);

void button_set_pos(button* o, vec2f pos);
void button_set_size(button* o, vec2f size);
void button_set_font_size(button* o, vec2f font_size);
void button_set_font(button* o, font* fnt);
void button_set_text(button* o, const char* text);
void button_set_text_color(button* o, vec3f clr);
void button_set_text_color_pressed(button* o, vec3f clr);

#endif
