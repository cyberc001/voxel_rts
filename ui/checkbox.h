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

#endif
