#include "ui/itembox.h"
#include "render/font.h"

#define UPDATE_SCROLLBAR(){\
	if(o->sb){\
		scrollbar_set_slider_max(o->sb, max(0, o->items.busy - (o->size.y - o->corner_size.y*2) / o->item_h) + 1);\
		o->sb->rerender = 1;\
	}\
}
#define RERENDER(){\
	render_obj_free(&o->ro_atex);\
	o->ro_atex = ui_render_full_box((ui_element*)o, o->corner_size, o->box_tex);\
	vec2f item_pos = {o->pos.x + o->corner_size.x, o->pos.y + o->size.y - o->corner_size.y - o->item_h};\
	for(size_t i = 0; i < o->items.busy; ++i){\
		render_obj_free(&o->items.data[i].ro_atex); render_obj_free(&o->items.data[i].ro_atex_s);\
		render_obj_free(&o->items.data[i].ro_text); render_obj_free(&o->items.data[i].ro_text_s);\
		ui_element ue;\
		ue.pos = item_pos; ue.size = (vec2f){o->size.x - o->corner_size.x*2, o->item_h};\
		o->items.data[i].ro_atex = ui_render_lrm_box(&ue, o->item_tex);\
		o->items.data[i].ro_atex_s = ui_render_lrm_box(&ue, o->item_tex_s);\
		o->items.data[i].ro_text = ui_render_aligned_text(&ue, o->text_fnt, o->items.data[i].str,  o->text_size, o->text_clr, UI_ALIGN_CENTER_V | UI_ALIGN_CENTER_H);\
		o->items.data[i].ro_text_s = ui_render_aligned_text(&ue, o->text_fnt, o->items.data[i].str,  o->text_size, o->text_select_clr, UI_ALIGN_CENTER_V | UI_ALIGN_CENTER_H);\
		item_pos.y -= o->item_h;\
	}\
}

static void _itembox_render(ui_element* _o)
{
	itembox* o = (itembox*)_o;
	UI_ELEMENT_ON_RERENDER(RERENDER());
	render_obj_draw(&o->ro_atex);

	float offy = o->sb ? o->sb->slider_pos * o->item_h : 0;
	ui_offset(0, offy);
	render_push_bbox(vec2_add(o->pos, o->corner_size), (vec2f){o->size.x - o->corner_size.x*2, o->size.y - o->corner_size.y*2});
	for(size_t i = 0; i < o->items.busy; ++i)
		if(i == o->selection){
			render_obj_draw(&o->items.data[i].ro_atex_s);
			render_obj_draw(&o->items.data[i].ro_text_s);
		}
		else{
			render_obj_draw(&o->items.data[i].ro_atex);
			render_obj_draw(&o->items.data[i].ro_text);
		}
	render_pop_bbox();
	ui_offset(0, -offy);
}
static void _itembox_key_down(ui_element* _o, key_code key, int mods)
{
	itembox* o = (itembox*)_o;
	vec2f mc = get_mouse_coords();
	if(key == MOUSE_LEFT && is_point_in_rect2f(mc, (rect2f){vec2_add(o->pos, o->corner_size), (vec2f){o->size.x - o->corner_size.x*2, o->size.y - o->corner_size.y*2}})){
		float y = 1 - (mc.y - o->corner_size.y - o->pos.y) / o->size.y;
		size_t i = y * ((o->size.y - o->corner_size.y*2) / o->item_h + 1) - 1;
		i += o->sb->slider_pos;
		o->selection = i;
	}
}

void itembox_create(itembox* o, vec2f pos, vec2f size, vec2f corner_size,
			vec2f text_size, font* text_fnt, float item_h,
			struct ui_full_box_textures box_tex,
			struct ui_lrm_box_textures item_tex, struct ui_lrm_box_textures item_tex_s)
{
	ui_element_create((ui_element*)o, pos, size);
	vt_init(o->vt, vt_insert(&vt, ui_element_render, _itembox_render);
			vt_insert(&vt, ui_element_key_down, _itembox_key_down));

	o->orig_size = size;
	o->corner_size = corner_size;
	o->text_size = text_size;
	o->text_clr = (vec3f){0, 0, 0}; o->text_select_clr = (vec3f){1, 1, 1};
	o->text_fnt = text_fnt; o->item_h = item_h;

	o->box_tex = box_tex; o->item_tex = item_tex; o->item_tex_s = item_tex_s;

	o->selection = (size_t)-1;
	o->sb = NULL;

	itembox_arr_create(&o->items);

	o->ro_atex = RENDER_OBJ_EMPTY;
	RERENDER();
}

void itembox_set_scrollbar(itembox* o, scrollbar* sb)
{
	o->size.x = o->orig_size.x - sb->size.x;
	sb->pos = (vec2f){ o->pos.x + o->size.x, o->pos.y };
	sb->size.y = o->size.y;

	o->sb = sb;
	scrollbar_set_slider_min(o->sb, 0);
	UPDATE_SCROLLBAR();
	o->rerender = 1;
}

size_t itembox_add_item(itembox* o, char* item)
{
	UPDATE_SCROLLBAR();
	return itembox_arr_push(&o->items, (itembox_item){item, RENDER_OBJ_EMPTY, RENDER_OBJ_EMPTY, RENDER_OBJ_EMPTY, RENDER_OBJ_EMPTY});
}
void itembox_delete_item(itembox* o, size_t i)
{
	itembox_arr_delete(&o->items, i);
}
char* itembox_get_selected_item(itembox* o)
{
	if(o->selection < o->items.busy)
		return o->items.data[o->selection].str;
	return NULL;
}
