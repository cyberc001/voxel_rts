#include "ui/label.h"

#define RERENDER(){\
	if(o->rendered_already)\
		o->rendered_already = 0;\
	else{\
		render_obj_free(&o->ro_text);\
		float text_h;\
		o->orig_pos = o->pos;\
		if(o->wrap) o->ro_text = render_text(o->fnt, o->text, o->pos, o->font_size, o->clr, RENDER_TEXT_WRAP, o->wrap, o->wrap_w, RENDER_TEXT_OUT_H, &text_h, RENDER_TEXT_OPTIONS_END);\
		else o->ro_text = render_text(o->fnt, o->text, o->pos, o->font_size, o->clr, RENDER_TEXT_OUT_H, &text_h, RENDER_TEXT_OPTIONS_END);\
		if(o->in_listbox){\
			o->pos.y = o->orig_pos.y + text_h/2 - text_height(o->fnt, o->text, o->font_size);\
			if(o->wrap) o->ro_text = render_text(o->fnt, o->text, o->pos, o->font_size, o->clr, RENDER_TEXT_WRAP, o->wrap, o->wrap_w, RENDER_TEXT_OPTIONS_END);\
			else o->ro_text = render_text(o->fnt, o->text, o->pos, o->font_size, o->clr, RENDER_TEXT_OPTIONS_END);\
			o->size.y = text_h / 2;\
			if(o->parent){\
				o->rendered_already = 1;\
				o->parent->rerender = 1;\
			}\
		}\
	}\
}
static void _label_render(ui_element* _o)
{
	label* o = (label*)_o;
	UI_ELEMENT_ON_RERENDER(RERENDER());
	render_obj_draw(&o->ro_text);
}

void label_create(label* o, vec2f pos, vec2f font_size, font* fnt, const char* text)
{
	ui_element_create((ui_element*)o, pos, (vec2f){0, 0});
	vt_init(o->vt, vt_insert(&vt, ui_element_render, _label_render));

	o->fnt = fnt;
	o->text = text;
	o->clr = (vec3f){1, 1, 1};
	o->font_size = font_size;

	o->rendered_already = 0;

	o->ro_text = RENDER_OBJ_EMPTY;
	RERENDER();
}

void label_set_pos(label* o, vec2f pos)
{
	o->pos = pos;
	o->rerender = 1;
}
void label_set_font_size(label* o, vec2f font_size)
{
	o->font_size = font_size;
	o->rerender = 1;
}
void label_set_font(label* o, font* fnt)
{
	o->fnt = fnt;
	o->rerender = 1;
}
void label_set_text(label* o, const char* text)
{
	o->text = text;
	o->rerender = 1;
}
void label_set_color(label* o, vec3f clr)
{
	o->clr = clr;
	o->rerender = 1;
}
void label_set_wrap(label* o, int wrap, float wrap_w)
{
	o->wrap = wrap;
	o->wrap_w = wrap_w;
	o->rerender = 1;
}
void label_set_in_listbox(label* o, int in_listbox)
{
	o->in_listbox = in_listbox;
	o->rerender = 1;
}
