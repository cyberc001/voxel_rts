#include "ui/button.h"
#include "more_math.h"

#define RERENDER_ALL(){\
	RERENDER_TEXT(); RERENDER_ATEX();\
}
#define RERENDER_TEXT(){\
	o->ro_text = ui_render_aligned_text((ui_element*)o, o->fnt, o->text, o->font_size, o->text_clr, UI_ALIGN_CENTER_BOTH);\
	o->ro_text_p = ui_render_aligned_text((ui_element*)o, o->fnt, o->text, o->font_size, o->text_clr_p, UI_ALIGN_CENTER_BOTH);\
}
#define RERENDER_ATEX(){\
	render_obj_free(&o->ro_atex); render_obj_free(&o->ro_atex_p);\
	o->ro_atex = ui_render_lrm_box((ui_element*)o, o->left, o->right, o->mid);\
	o->ro_atex_p = ui_render_lrm_box((ui_element*)o, o->left_p, o->right_p, o->mid_p);\
}

static void _button_render(ui_element* _o)
{
	button* o = (button*)_o;
	UI_ELEMENT_ON_RERENDER(RERENDER_ALL());
	if(o->pressed){
		render_obj_draw(&o->ro_atex_p);
		render_obj_draw(&o->ro_text_p);
	}
	else{
		render_obj_draw(&o->ro_atex);
		render_obj_draw(&o->ro_text);
	}
}
static void _button_key_down(ui_element* _o, key_code key, int mods)
{
	button* o = (button*)_o;
	if(key == MOUSE_LEFT && is_point_in_rect2f(get_mouse_coords(), o->pos, o->size))
		o->pressed = 1;
}
static void _button_key_up(ui_element* _o, key_code key, int mods)
{
	button* o = (button*)_o;
	if(key == MOUSE_LEFT)
		o->pressed = 0;
}

void button_create(button* o, vec2f pos, vec2f size, vec2f font_size,
			atlas_texture* left, atlas_texture* right, atlas_texture* mid,
			atlas_texture* left_pressed, atlas_texture* right_pressed, atlas_texture* mid_pressed,
			font* fnt, const char* text)
{
	ui_element_create((ui_element*)o, pos, size);
	vt_init(o->vt, vt_insert(&vt, ui_element_render, _button_render);
			vt_insert(&vt, ui_element_key_down, _button_key_down);
			vt_insert(&vt, ui_element_key_up, _button_key_up));

	o->font_size = font_size;
	o->fnt = fnt;
	o->text = text;
	o->text_clr = (vec3f){0, 0, 0};
	o->text_clr_p = (vec3f){0.8, 0.8, 0.8};

	o->left = left; o->right = right; o->mid = mid;
	o->left_p = left_pressed; o->right_p = right_pressed; o->mid_p = mid_pressed;

	o->ro_text = o->ro_text_p = o->ro_atex = o->ro_atex_p = RENDER_OBJ_EMPTY;
	RERENDER_ALL();

	o->pressed = 0;
}

void button_set_pos(button* o, vec2f pos)
{
	o->pos = pos;
	o->rerender = 1;
}
void button_set_size(button* o, vec2f size)
{
	o->size = size;
	o->rerender = 1;
}
void button_set_font_size(button* o, vec2f size)
{
	o->size = size;
	o->rerender = 1; //RERENDER_TEXT();
}
void button_set_font(button* o, font* fnt)
{
	o->fnt = fnt;
	o->rerender = 1; //RERENDER_TEXT();
}
void button_set_text(button* o, const char* text)
{
	o->text = text;
	o->rerender = 1; //RERENDER_TEXT();
}
void button_set_text_color(button* o, vec3f clr)
{
	o->text_clr = clr;
	o->rerender = 1; //RERENDER_TEXT();
}
void button_set_text_color_pressed(button* o, vec3f clr)
{
	o->text_clr_p = clr;
	o->rerender = 1; //RERENDER_TEXT();
}
