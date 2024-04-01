#ifndef HSLIDER_H
#define HSLIDER_H

#include "ui/ui_element.h"
#include "render/texture.h"

typedef struct hslider hslider;
struct hslider {
	struct ui_element;

	atlas_texture *line, *tick, *slider;
	float line_h, tick_h, slider_h;
	float slider_w; // calculated

	int slider_pos, slider_min, slider_max;
	int tick_val; // (slider_max - slider_min + 1) / tick_val = tick count
	int dragging_slider;

	render_obj ro_atex, ro_slider;
};

void hslider_create(hslider* o, vec2f pos, vec2f size,
			atlas_texture* line, atlas_texture* tick, atlas_texture* slider,
			float line_h, float tick_h, float slider_h,
			int tick_val, int slider_min, int slider_max);

#endif
