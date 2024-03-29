#include "ui/scrollbar.h"
#include "more_math.h"

#define RERENDER(){\
	o->up_h = atlas_texture_aspect_h(o->up, o->size.x);\
	o->down_h = atlas_texture_aspect_h(o->down, o->size.x);\
	o->mid_h = o->size.y - o->up_h - o->down_h;\
	o->slider_h = atlas_texture_aspect_h(o->slider, o->size.x);\
	render_obj_free(&o->ro_atex); render_obj_free(&o->ro_slider);\
	o->ro_atex = ui_render_udm_box((ui_element*)o, o->up, o->down, o->mid);\
	o->ro_slider = render_atlas_texture(o->slider, (vec2f){o->pos.x, o->pos.y + o->down_h}, (vec2f){o->size.x, o->slider_h});\
}

static void _scrollbar_render(ui_element* _o)
{
	scrollbar* o = (scrollbar*)_o;
	UI_ELEMENT_ON_RERENDER(RERENDER());
	render_obj_draw(&o->ro_atex);

	if(o->slider_min < o->slider_max){
		float slider_y = (o->mid_h - o->slider_h) * (1 - (o->slider_pos - o->slider_min) / (float)(o->slider_max - o->slider_min));
		ui_offset(0, slider_y);
		render_obj_draw(&o->ro_slider);
		ui_offset(0, -slider_y);
	}
}
static void _scrollbar_key_down(ui_element* _o, key_code key, int mods)
{
	scrollbar* o = (scrollbar*)_o;
	if(key == MOUSE_LEFT){
		vec2f up_off = {0, o->size.y - o->up_h};
		vec2f mc = get_mouse_coords();
		if(is_point_in_rect2f(mc, (rect2f){vec2_add(o->pos, up_off), (vec2f){o->size.x, o->up_h}}))
			o->slider_pos = max(o->slider_min, o->slider_pos - 1);
		else if(is_point_in_rect2f(mc, (rect2f){o->pos, (vec2f){o->size.x + o->up_h + o->mid_h, o->down_h}}))
			o->slider_pos = min(o->slider_max, o->slider_pos + 1);
		else{
			float slider_y = (o->mid_h - o->slider_h) * (1 - (o->slider_pos - o->slider_min) / (float)(o->slider_max - o->slider_min));
			if(is_point_in_rect2f(mc, (rect2f){(vec2f){o->pos.x, o->pos.y + o->down_h + slider_y}, (vec2f){o->size.x, o->slider_h}}))
				o->dragging_slider = 1;
		}
	}
}
static void _scrollbar_key_up(ui_element* _o, key_code key, int mods)
{
	scrollbar* o = (scrollbar*)_o;
	if(key == MOUSE_LEFT)
		o->dragging_slider = 0;
}
static void _scrollbar_mouse_move(ui_element* _o, float x, float y)
{
	scrollbar* o = (scrollbar*)_o;
	if(o->dragging_slider){
		float slider_y = 1 - clamp01(y, o->pos.y + o->down_h, o->pos.y + o->down_h + o->mid_h);
		o->slider_pos = o->slider_min + (o->slider_max - o->slider_min) * slider_y;
	}
}

void scrollbar_create(scrollbar* o, vec2f pos, vec2f size,
			atlas_texture* up, atlas_texture* down, atlas_texture* mid, atlas_texture* slider,
			int slider_min, int slider_max)
{
	ui_element_create((ui_element*)o, pos, size);
	vt_init(o->vt, vt_insert(&vt, ui_element_render, _scrollbar_render);
			vt_insert(&vt, ui_element_key_down, _scrollbar_key_down);
			vt_insert(&vt, ui_element_key_up, _scrollbar_key_up);
			vt_insert(&vt, ui_element_mouse_move, _scrollbar_mouse_move));

	o->up = up; o->down = down; o->mid = mid; o->slider = slider;
	o->slider_min = slider_min; o->slider_max = slider_max;
	o->dragging_slider = 0;

	o->ro_atex = o->ro_slider = RENDER_OBJ_EMPTY;
	RERENDER();
}

void scrollbar_set_pos(scrollbar* o, vec2f pos)
{
	o->pos = pos;
	o->rerender = 1;
}
void scrollbar_set_size(scrollbar* o, vec2f size)
{
	o->size = size;
	o->rerender = 1;
}
void scrollbar_set_slider_pos(scrollbar* o, int pos)
{
	o->slider_pos = clamp(pos, o->slider_min, o->slider_max);
}
void scrollbar_set_slider_min(scrollbar* o, int _min)
{
	float slider_y = o->slider_max == o->slider_min ? 0 : (o->slider_pos - o->slider_min) / (float)(o->slider_max - o->slider_min);
	o->slider_min = _min;
	o->slider_pos = o->slider_min + slider_y * (o->slider_max - o->slider_min);
}
void scrollbar_set_slider_max(scrollbar* o, int _max)
{
	float slider_y = o->slider_max == o->slider_min ? 0 : (o->slider_pos - o->slider_min) / (float)(o->slider_max - o->slider_min);
	o->slider_max = _max;
	o->slider_pos = o->slider_min + slider_y * (o->slider_max - o->slider_min);
}
