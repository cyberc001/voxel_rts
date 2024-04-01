#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include "ui/ui_element.h"
#include "render/texture.h"

typedef struct scrollbar scrollbar;
struct scrollbar {
	struct ui_element;

	atlas_texture *up, *down, *mid, *slider;
	float up_h, down_h, mid_h, slider_h;

	int slider_pos, slider_min, slider_max;
	int dragging_slider;

	render_obj ro_atex, ro_slider;
};

void scrollbar_create(scrollbar* o, vec2f pos, vec2f size,
			atlas_texture* up, atlas_texture* down, atlas_texture* mid, atlas_texture* slider,
			int slider_min, int slider_max);

void scrollbar_set_pos(scrollbar* o, vec2f pos);
void scrollbar_set_size(scrollbar* o, vec2f size);
void scrollbar_set_slider_pos(scrollbar* o, int pos);
void scrollbar_set_slider_min(scrollbar* o, int _min);
void scrollbar_set_slider_max(scrollbar* o, int _max);

#endif
