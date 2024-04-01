#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "ui/ui_element.h"
#include "render/font.h"
#include "render/texture.h"

typedef struct checkbox checkbox;
struct checkbox {
	struct ui_element;

	vec2f font_size; vec3f text_clr; font* fnt;
	const char* text;

	atlas_texture *check_tex, *uncheck_tex;
	render_obj ro_check, ro_uncheck, ro_text;

	int checked;
};

void checkbox_create(checkbox* o, vec2f post, vec2f size, vec2f font_size,
			font* fnt, const char* text,
			atlas_texture* check_tex, atlas_texture* uncheck_tex);

void checkbox_set_pos(checkbox* o, vec2f pos);
void checkbox_set_size(checkbox* o, vec2f size);
void checkbox_set_font_size(checkbox* o, vec2f font_size);
void checkbox_set_font(checkbox* o, font* fnt);
void checkbox_set_text(checkbox* o, const char* text);
void checkbox_set_text_color(checkbox* o, vec3f clr);

#endif
