#include "ui/ui_element.h"
#include "render/font.h"

void _ui_element_render(ui_element* o)
{
	ui_element_list_node* n = o->child_list.head.next;
	while(n != &o->child_list.tail){
		vt_call(*(n->data), ui_element_render);
		n = n->next;
	}
}
void _ui_element_key_down(ui_element* o, key_code key, int mods)
{
	ui_element_list_node* n = o->child_list.head.next;
	vec2f mc = get_mouse_coords();
	while(n != &o->child_list.tail){
		if(is_point_in_rect2f(mc, n->data->pos, n->data->size) && key_code_is_mouse(key)){
			if(o->child_focused)
				o->child_focused->focused = 0;
			n->data->focused = 1;
			o->child_focused = n->data;
		}
		vt_call(*(n->data), ui_element_key_down, key, mods);
		n = n->next;
	}
}
void _ui_element_key_up(ui_element* o, key_code key, int mods)
{
	ui_element_list_node* n = o->child_list.head.next;
	while(n != &o->child_list.tail){
		vt_call(*(n->data), ui_element_key_up, key, mods);
		n = n->next;
	}
}
void _ui_element_key_char(ui_element* o, utf_char c)
{
	ui_element_list_node* n = o->child_list.head.next;
	while(n != &o->child_list.tail){
		vt_call(*(n->data), ui_element_key_char, c);
		n = n->next;
	}
}
void _ui_element_mouse_move(ui_element* o, float x, float y)
{
	ui_element_list_node* n = o->child_list.head.next;
	while(n != &o->child_list.tail){
		vt_call(*(n->data), ui_element_mouse_move, x, y);
		n = n->next;
	}
}

void ui_element_create(ui_element* o, vec2f pos, vec2f size)
{
	vt_init(NULL, vt_insert(&vt, ui_element_render, _ui_element_render);
			vt_insert(&vt, ui_element_key_down, _ui_element_key_down);
			vt_insert(&vt, ui_element_key_up, _ui_element_key_up);
			vt_insert(&vt, ui_element_key_char, _ui_element_key_char);
			vt_insert(&vt, ui_element_mouse_move, _ui_element_mouse_move));
	o->prev_pos = o->pos = pos;
	o->size = size;
	o->rerender = o->focused = 0;

	o->parent = NULL;
	ui_element_list_create(&o->child_list);
	o->child_focused = NULL;
}

void ui_element_add_child(ui_element* o, ui_element* child)
{
	ui_element_list_insert_end(&o->child_list, child);
	child->parent = o;
}

/* Common functions */
render_obj ui_render_lrm_box(ui_element* o, struct ui_lrm_box_textures tex)
{
	atlas_texture* arr_tex[3] = {tex.l, tex.r, tex.m};
	float left_w = atlas_texture_aspect_w(tex.l, o->size.y);
	float right_w = atlas_texture_aspect_w(tex.r, o->size.y);
	vec2f right_edge = {o->pos.x + o->size.x, o->pos.y};
	vec2f left_size = {left_w, o->size.y};
	vec2f right_off = {right_w, 0}, right_size = {right_w, o->size.y};
	vec2f arr_coords[3] = {o->pos, vec2_sub(right_edge, right_off), {o->pos.x + left_w, o->pos.y}};
	vec2f arr_size[3] = {left_size, right_size, {o->size.x - left_w - right_w, o->size.y}};
	return render_atlas_textures(arr_tex, arr_coords, arr_size, 3);
}
render_obj ui_render_udm_box(ui_element* o, atlas_texture* u, atlas_texture* d, atlas_texture* m)
{
	atlas_texture* arr_tex[3] = {d, m, u};
	float up_h = atlas_texture_aspect_h(u, o->size.x);
	float down_h = atlas_texture_aspect_h(d, o->size.x);
	float mid_h = o->size.y - up_h - down_h;
	vec2f down_size = {o->size.x, down_h};
	vec2f mid_size = {o->size.x, mid_h}, mid_off = {0, down_h};
	vec2f up_size = {o->size.x, up_h}, up_off = {0, down_h + mid_h};

	vec2f arr_coords[3] = {o->pos, vec2_add(o->pos, mid_off), vec2_add(o->pos, up_off)};
	vec2f arr_size[3] = {down_size, mid_size, up_size};
	return render_atlas_textures(arr_tex, arr_coords, arr_size, 3);
}
render_obj ui_render_full_box(ui_element* o, vec2f corner_size, struct ui_full_box_textures tex)
{
	atlas_texture* arr_tex[9] = {tex.lb, tex.d, tex.rb, tex.r, tex.rt, tex.u, tex.lt, tex.l, tex.m};

	vec2f lb_off = {corner_size.x, 0};
	vec2f d_off = {o->size.x - corner_size.x, 0};
	vec2f rb_off = {o->size.x - corner_size.x, corner_size.y};
	vec2f r_off = {o->size.x - corner_size.x, o->size.y - corner_size.y};
	vec2f rt_off = {corner_size.x, o->size.y - corner_size.y};
	vec2f u_off = {0, o->size.y - corner_size.y};
	vec2f lt_off = {0, corner_size.y};

	vec2f arr_coords[9] = {o->pos, vec2_add(o->pos, lb_off), vec2_add(o->pos, d_off),
				vec2_add(o->pos, rb_off), vec2_add(o->pos, r_off), vec2_add(o->pos, rt_off),
				vec2_add(o->pos, u_off), vec2_add(o->pos, lt_off),
				vec2_add(o->pos, corner_size)};
	vec2f arr_size[9] = {corner_size, {o->size.x - corner_size.x*2, corner_size.y}, corner_size,
				{corner_size.x, o->size.y - corner_size.y*2}, corner_size, {o->size.x - corner_size.x*2, corner_size.y},
				corner_size, {corner_size.x, o->size.y - corner_size.y*2},
				{o->size.x - corner_size.x*2, o->size.y - corner_size.y*2}};
	return render_atlas_textures(arr_tex, arr_coords, arr_size, 9);
}

render_obj ui_render_aligned_text(ui_element* o, font* fnt, const char* text, vec2f font_size, vec3f clr, int flags)
{
	vec2f text_pos = {o->pos.x
			+ (flags & UI_ALIGN_CENTER_H ? o->size.x / 2 - text_width(fnt, text, font_size) / 2 : 0)
			+ (flags & UI_ALIGN_RIGHT ? o->size.x - text_width(fnt, text, font_size) : 0),
			o->pos.y
			+ (flags & UI_ALIGN_CENTER_V ? o->size.y / 2 - text_height(fnt, text, font_size) / 2 : 0)};
	return render_text(fnt, text, text_pos, font_size, clr);
}

void ui_offset(float x, float y)
{
	vec2f s = {x, y};
	TRANSLATE_ORTHO_SIZE(s);
	glTranslatef(s.x, s.y, 0);
}
