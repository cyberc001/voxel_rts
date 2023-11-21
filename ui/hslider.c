#include "ui/hslider.h"
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
	arr_size[tick_cnt] = (vec2f){o->size.x, o->line_h};\
	vec2f tickpos = o->pos;\
	vec2f ticksize = {atlas_texture_aspect_w(o->tick, o->tick_h), o->tick_h};\
	float tickstep = o->size.x / (o->slider_max - o->slider_min) * o->tick_val;\
	for(size_t i = 0; i < tick_cnt; ++i){\
		if(i == tick_cnt - 1) tickpos.x = o->pos.x + o->size.x - ticksize.x;\
		arr_tex[i] = o->tick;\
		arr_coords[i] = tickpos;\
		arr_size[i] = ticksize;\
		tickpos.x += tickstep;\
	}\
	o->ro_atex = render_atlas_textures(arr_tex, arr_coords, arr_size, tick_cnt + 1);\
	o->slider_w = atlas_texture_aspect_w(o->slider, o->slider_h);\
	o->ro_slider = render_atlas_texture(o->slider, (vec2f){o->pos.x, o->pos.y - o->slider_h + o->line_h}, (vec2f){o->slider_w, o->slider_h});\
}

static void _hslider_render(ui_element* _o)
{
	hslider* o = (hslider*)_o;
	UI_ELEMENT_ON_RERENDER(RERENDER());
	render_obj_draw(&o->ro_atex);

	if(o->slider_min < o->slider_max){
		o->slider_pos = clamp(o->slider_pos, o->slider_min, o->slider_max);
		float slider_x = o->size.x * ((o->slider_pos - o->slider_min) / (float)(o->slider_max - o->slider_min));
		if(o->slider_pos == o->slider_max) slider_x -= o->slider_w;
		//else if(o->slider_pos != o->slider_min)	slider_x -= o->slider_w / 2;
		ui_offset(slider_x, 0);
		render_obj_draw(&o->ro_slider);
		ui_offset(-slider_x, 0);
	}
}

static void _hslider_key_down(ui_element* _o, key_code key, int mods)
{
	hslider* o = (hslider*)_o;
	if(key == MOUSE_LEFT){
		vec2f mc = get_mouse_coords();
		float slider_x = o->size.x * ((o->slider_pos - o->slider_min) / (float)(o->slider_max - o->slider_min));
		if(is_point_in_rect2f(mc, (vec2f){o->pos.x + slider_x, o->pos.y - o->slider_h + o->line_h}, (vec2f){o->slider_w, o->slider_h}))
			o->dragging_slider = 1;
	}
}
static void _hslider_key_up(ui_element* _o, key_code key, int mods)
{
	hslider* o = (hslider*)_o;
	if(key == MOUSE_LEFT)
		o->dragging_slider = 0;
}
static void _hslider_mouse_move(ui_element* _o, float x, float y)
{
	hslider* o = (hslider*)_o;
	if(o->dragging_slider){
		float slider_x = clamp01(x, o->pos.x, o->pos.x + o->size.x - o->slider_w);
		o->slider_pos = o->slider_min + (o->slider_max - o->slider_min) * slider_x;
	}
}

void hslider_create(hslider* o, vec2f pos, vec2f size,
			atlas_texture* line, atlas_texture* tick, atlas_texture* slider,
			float line_h, float tick_h, float slider_h,
			int tick_val, int slider_min, int slider_max)
{
	ui_element_create((ui_element*)o, pos, size);
	vt_init(o->vt, vt_insert(&vt, ui_element_render, _hslider_render);
			vt_insert(&vt, ui_element_key_down, _hslider_key_down);
			vt_insert(&vt, ui_element_key_up, _hslider_key_up);
			vt_insert(&vt, ui_element_mouse_move, _hslider_mouse_move));

	o->line = line; o->tick = tick; o->slider = slider;
	o->line_h = line_h; o->tick_h = tick_h; o->slider_h = slider_h;
	o->tick_val = tick_val; o->slider_min = slider_min; o->slider_max = slider_max;

	o->ro_atex = o->ro_slider = RENDER_OBJ_EMPTY;
	RERENDER();
}
