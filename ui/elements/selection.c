#include "ui/elements/selection.h"
#include "render/base.h"
#include "controls/selection.h"
#include "controls.h"
#include "ui/ui_element.h"

#define RERENDER(){\
	vec2f mcoords = get_mouse_coords();\
	vec2f sel_start = controls_selection_start;\
	sel_start.x = (sel_start.x + 1) / 2;\
	sel_start.y = (sel_start.y + 1) / 2;\
	vec2f selbox_min = {min(sel_start.x, mcoords.x),\
				min(sel_start.y, mcoords.y)},\
		selbox_max = {max(sel_start.x, mcoords.x),\
				max(sel_start.y, mcoords.y)};\
	ui_element ue = {\
		.pos = selbox_min,\
		.size = vec2_sub(selbox_max, selbox_min)\
	};\
	o->sel_robj = ui_render_full_box(&ue, (vec2f){0.005, 0.005}, o->box_tex);\
}

static void _selection_render(ui_element* _o)
{
	selection* o = (selection*)_o;
	if(controls_is_selecting){
		RERENDER();
		render_obj_draw(&o->sel_robj);
	}
}

void selection_create(selection* o, struct ui_full_box_textures box_tex)
{
	ui_element_create((ui_element*)o, (vec2f){0, 0}, (vec2f){0, 0});
	vt_init(o->vt, vt_insert(&vt, ui_element_render, _selection_render));	
	o->box_tex = box_tex;
	o->sel_robj = RENDER_OBJ_EMPTY;
}
