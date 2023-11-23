#include "ui/checkbox.h"

#define RERENDER(){\
	render_obj_free(&o->ro_check);\
	render_obj_free(&o->ro_uncheck);\
	render_obj_free(&o->ro_text);\
	vec2f check_pos = {o->pos.x, o->pos.y + o->size.y/2 - o->size.y/2};\
	o->ro_check = render_atlas_texture(o->check_tex, check_pos, o->size);\
	o->ro_uncheck = render_atlas_texture(o->uncheck_tex, check_pos, o->size);\
	check_pos.x += o->size.x * 3 / 2;\
	ui_element ue = {.pos = check_pos, .size = (vec2f){o->size.x - o->size.x*3/2, o->size.y}};\
	o->ro_text = ui_render_aligned_text(&ue, o->fnt, o->text, o->font_size, o->text_clr, UI_ALIGN_CENTER_V);\
}

static void _checkbox_render(ui_element* _o)
{
	checkbox* o = (checkbox*)_o;
	UI_ELEMENT_ON_RERENDER(RERENDER());
	if(o->checked)
		render_obj_draw(&o->ro_check);
	else
		render_obj_draw(&o->ro_uncheck);
	render_obj_draw(&o->ro_text);
}
static void _checkbox_key_down(ui_element* _o, key_code key, int mods)
{
	checkbox* o = (checkbox*)_o;
	if(key == MOUSE_LEFT && is_point_in_rect2f(get_mouse_coords(), o->pos, o->size))
		o->checked = !o->checked;
}

void checkbox_create(checkbox* o, vec2f pos, vec2f size, vec2f font_size,
			font* fnt, const char* text,
			atlas_texture* check_tex, atlas_texture* uncheck_tex)
{
	ui_element_create((ui_element*)o, pos, size);
	vt_init(o->vt, vt_insert(&vt, ui_element_render, _checkbox_render);
			vt_insert(&vt, ui_element_key_down, _checkbox_key_down));

	o->font_size = font_size;
	o->fnt = fnt;
	o->text = text;
	o->text_clr = (vec3f){0.8, 0.8, 0.8};

	o->check_tex = check_tex; o->uncheck_tex = uncheck_tex;

	o->checked = 0;
	RERENDER();
}

void checkbox_set_pos(checkbox* o, vec2f pos)
{
	o->pos = pos;
	o->rerender = 1;
}
void checkbox_set_size(checkbox* o, vec2f size)
{
	o->size = size;
	o->rerender = 1;
}
void checkbox_set_font_size(checkbox* o, vec2f font_size)
{
	o->font_size = font_size;
	o->rerender = 1;
}
void checkbox_set_font(checkbox* o, font* fnt)
{
	o->fnt = fnt;
	o->rerender = 1;
}
void checkbox_set_text(checkbox* o, const char* text)
{
	o->text = text;
	o->rerender = 1;
}
void checkbox_set_text_color(checkbox* o, vec3f clr)
{
	o->text_clr = clr;
	o->rerender = 1;
}
