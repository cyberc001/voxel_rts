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
	atlas_texture *left, *right, *mid;
	atlas_texture *left_p, *right_p, *mid_p;

	render_obj ro_text, ro_text_p,
		   ro_atex, ro_atex_p;

	int pressed;
};

void button_create(button* o, vec2f pos, vec2f size, vec2f font_size,
			atlas_texture* left, atlas_texture* right, atlas_texture* mid,
			atlas_texture* left_pressed, atlas_texture* right_pressed, atlas_texture* mid_pressed,
			font* fnt, const char* text);

void button_set_pos(button* o, vec2f pos);
void button_set_size(button* o, vec2f size);
void button_set_font_size(button* o, vec2f font_size);
void button_set_font(button* o, font* fnt);
void button_set_text(button* o, const char* text);
void button_set_text_color(button* o, vec3f clr);
void button_set_text_color_pressed(button* o, vec3f clr);

#endif
