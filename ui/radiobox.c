#include "ui/radiobox.h"

void radiobox_group_create(radiobox_group* rg)
{
	radiobox_group_arr_create(&rg->arr);
	rg->selection = NULL;
}
size_t radiobox_group_push(radiobox_group* rg, radiobox* rb)
{
	radiobox_set_group(rb, rg);
	return radiobox_group_arr_push(&rg->arr, rb);
}
void radiobox_group_delete(radiobox_group* rg, size_t i)
{
	radiobox_group_arr_delete(&rg->arr, i);
}
void radiobox_group_delete_last(radiobox_group* rg)
{
	radiobox_group_arr_delete_last(&rg->arr);
}
void radiobox_group_select(radiobox_group* rg, radiobox* rb)
{
	if(rg->selection)
		rg->selection->checked = 0;
	rb->checked = 1;
	rg->selection = rb;
}
void radiobox_group_free(radiobox_group* rg);

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

static void _radiobox_render(ui_element* _o)
{
	radiobox* o = (radiobox*)_o;
	UI_ELEMENT_ON_RERENDER(RERENDER());
	if(o->checked)
		render_obj_draw(&o->ro_check);
	else
		render_obj_draw(&o->ro_uncheck);
	render_obj_draw(&o->ro_text);
}
static void _radiobox_key_down(ui_element* _o, key_code key, int mods)
{
	radiobox* o = (radiobox*)_o;
	if(key == MOUSE_LEFT && is_point_in_rect2f(get_mouse_coords(), o->pos, o->size)){
		if(o->group)
			radiobox_group_select(o->group, o);
		else
			o->checked = !o->checked;
	}
}

void radiobox_create(radiobox* o, vec2f pos, vec2f size, vec2f font_size,
			font* fnt, const char* text,
			atlas_texture* check_tex, atlas_texture* uncheck_tex)
{
	ui_element_create((ui_element*)o, pos, size);
	vt_init(o->vt, vt_insert(&vt, ui_element_render, _radiobox_render);
			vt_insert(&vt, ui_element_key_down, _radiobox_key_down));

	o->font_size = font_size;
	o->fnt = fnt;
	o->text = text;
	o->text_clr = (vec3f){0.8, 0.8, 0.8};

	o->check_tex = check_tex; o->uncheck_tex = uncheck_tex;

	o->checked = 0;
	o->group = NULL;
	RERENDER();
}

void radiobox_set_group(radiobox* o, radiobox_group* rg)
{
	if(o->group && o->group->selection == o)
		o->group->selection = NULL;
	if(!rg->selection)
		radiobox_group_select(rg, o);
	o->group = rg;
}
void radiobox_set_pos(radiobox* o, vec2f pos)
{
	o->pos = pos;
	o->rerender = 1;
}
void radiobox_set_size(radiobox* o, vec2f size)
{
	o->size = size;
	o->rerender = 1;
}
void radiobox_set_font_size(radiobox* o, vec2f font_size)
{
	o->font_size = font_size;
	o->rerender = 1;
}
void radiobox_set_font(radiobox* o, font* fnt)
{
	o->fnt = fnt;
	o->rerender = 1;
}
void radiobox_set_text(radiobox* o, const char* text)
{
	o->text = text;
	o->rerender = 1;
}
void radiobox_set_text_color(radiobox* o, vec3f clr)
{
	o->text_clr = clr;
	o->rerender = 1;
}
