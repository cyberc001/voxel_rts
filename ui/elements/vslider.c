#include "ui/elements/vslider.h"
#include "more_math.h"

#define RERENDER(){\
	render_obj_free(&o->ro_atex);\
	render_obj_free(&o->ro_slider);\
	o->tick_val = clamp(o->tick_val, 1, o->slider_max - o->slider_min);\
	size_t tick_cnt = ceil((float)(o->slider_max - o->slider_min + 1) / o->tick_val);\
	atlas_texture** arr_tex = malloc(sizeof(atlas_texture*) * (tick_cnt + 1));\
	vec2f* arr_coords = malloc(sizeof(vec2f) * (tick_cnt + 1));\
	vec2f* arr_size = malloc(sizeof(vec2f) * (tick_cnt + 1));\
	arr_tex[tick_cnt] = o->line;\
	arr_coords[tick_cnt] = o->pos;\
	arr_size[tick_cnt] = (vec2f){o->line_w, o->size.y};\
	vec2f tickpos = o->pos;\
	vec2f ticksize = {o->tick_w, atlas_texture_aspect_h(o->tick, o->tick_w)};\
	float tickstep = o->size.y / (o->slider_max - o->slider_min) * o->tick_val;\
	for(size_t i = 0; i < tick_cnt; ++i){\
		if(i == tick_cnt - 1) tickpos.y = o->pos.y + o->size.y - ticksize.y;\
		arr_tex[i] = o->tick;\
		arr_coords[i] = tickpos;\
		arr_size[i] = ticksize;\
		tickpos.y += tickstep;\
	}\
	o->ro_atex = render_atlas_textures(arr_tex, arr_coords, arr_size, tick_cnt + 1);\
	o->slider_h = atlas_texture_aspect_h(o->slider, o->slider_w);\
	o->ro_slider = render_atlas_texture(o->slider, (vec2f){o->pos.x, o->pos.y + o->size.y - o->slider_h}, (vec2f){o->slider_w, o->slider_h});\
}

static void _vslider_render(ui_element* _o)
{
	vslider* o = (vslider*)_o;
	UI_ELEMENT_ON_RERENDER(RERENDER());
	render_obj_draw(&o->ro_atex);

	if(o->slider_min < o->slider_max){
		o->slider_pos = clamp(o->slider_pos, o->slider_min, o->slider_max);
		float slider_y = o->size.y * ((o->slider_pos - o->slider_min) / (float)(o->slider_max - o->slider_min));
		if(o->slider_pos == o->slider_max) slider_y -= o->slider_h;
		ui_offset(0, -slider_y);
		render_obj_draw(&o->ro_slider);
		ui_offset(0, slider_y);
	}
}

static void _vslider_key_down(ui_element* _o, key_code key, int mods)
{
	vslider* o = (vslider*)_o;
	if(key == MOUSE_LEFT){
		vec2f mc = get_mouse_coords();
		float slider_y = o->size.y * (1 - (o->slider_pos - o->slider_min) / (float)(o->slider_max - o->slider_min));
		if(o->slider_pos == o->slider_max) slider_y -= o->slider_h;
		if(is_point_in_rect2f(mc, (rect2f){(vec2f){o->pos.x, o->pos.y + slider_y - o->slider_h}, (vec2f){o->slider_w, o->slider_h}}))
			o->dragging_slider = 1;
		else if(is_point_in_rect2f(mc, (rect2f){o->pos, o->size})){
			float slider_y = 1 - clamp01(mc.y, o->pos.y, o->pos.y + o->size.y - o->slider_h);
			o->slider_pos = o->slider_min + (o->slider_max - o->slider_min) * slider_y;
		}
	}
}
static void _vslider_key_up(ui_element* _o, key_code key, int mods)
{
	vslider* o = (vslider*)_o;
	if(key == MOUSE_LEFT)
		o->dragging_slider = 0;
}
static void _vslider_mouse_move(ui_element* _o, float x, float y)
{
	vslider* o = (vslider*)_o;
	if(o->dragging_slider){
		float slider_y = 1 - clamp01(y, o->pos.y, o->pos.y + o->size.y - o->slider_h);
		o->slider_pos = o->slider_min + (o->slider_max - o->slider_min) * slider_y;
	}
}

void vslider_create(vslider* o, vec2f pos, vec2f size,
			atlas_texture* line, atlas_texture* tick, atlas_texture* slider,
			float line_w, float tick_w, float slider_w,
			int tick_val, int slider_min, int slider_max)
{
	ui_element_create((ui_element*)o, pos, size);
	vt_init(o->vt, vt_insert(&vt, ui_element_render, _vslider_render);
			vt_insert(&vt, ui_element_key_down, _vslider_key_down);
			vt_insert(&vt, ui_element_key_up, _vslider_key_up);
			vt_insert(&vt, ui_element_mouse_move, _vslider_mouse_move));

	o->line = line; o->tick = tick; o->slider = slider;
	o->line_w = line_w; o->tick_w = tick_w; o->slider_w = slider_w;
	o->tick_val = tick_val; o->slider_min = slider_min; o->slider_max = slider_max;

	o->ro_atex = o->ro_slider = RENDER_OBJ_EMPTY;
	RERENDER();
}
