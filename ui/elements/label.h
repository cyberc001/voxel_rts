#ifndef LABEL_H
#define LABEL_H

#include "ui/ui_element.h"
#include "render/font.h"

typedef struct label label;
struct label {
	struct ui_element;
	
	// handling god damn ListBox
	int in_listbox;
	vec2f orig_pos;
	int rendered_already;

	font* fnt; vec2f font_size;
	const char* text;
	vec3f clr;
	int wrap; float wrap_w;

	render_obj ro_text;
};

void label_create(label* o, vec2f pos, vec2f font_size, font* fnt, const char* text);

void label_set_pos(label* o, vec2f pos);
void label_set_font_size(label* o, vec2f font_size);
void label_set_font(label* o, font* fnt);
void label_set_text(label* o, const char* text);
void label_set_color(label* o, vec3f clr);
void label_set_wrap(label* o, int wrap, float wrap_w); // see render/font.h
void label_set_in_listbox(label* o, int in_listbox);

#endif
