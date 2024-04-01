#ifndef VSLIDER_H
#define VSLIDER_H

#include "ui/ui_element.h"
#include "render/texture.h"

typedef struct vslider vslider;
struct vslider {
	struct ui_element;

	atlas_texture *line, *tick, *slider;
	float line_w, tick_w, slider_w;
	float slider_h; // calculated

	int slider_pos, slider_min, slider_max;
	int tick_val; // (slider_max - slider_min + 1) / tick_val = tick count
	int dragging_slider;

	render_obj ro_atex, ro_slider;
};

void vslider_create(vslider* o, vec2f pos, vec2f size,
			atlas_texture* line, atlas_texture* tick, atlas_texture* slider,
			float line_w, float tick_w, float slider_w,
			int tick_val, int slider_min, int slider_max);

#endif
