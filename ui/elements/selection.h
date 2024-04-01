#ifndef SELECTION_H
#define SELECTION_H

#include "ui/ui_element.h"

typedef struct selection selection;
struct selection {
	struct ui_element;

	ui_element sel_bounds;
	struct ui_full_box_textures box_tex;
	render_obj sel_robj;
};

void selection_create(selection* o, struct ui_full_box_textures box_tex);

#endif
