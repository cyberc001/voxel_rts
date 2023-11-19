#include "ui/listbox.h"

#define RERENDER(){\
	render_obj_free(&o->ro_atex);\
	o->ro_atex = ui_render_full_box((ui_element*)o, o->corner_size, o->box_tex);\
	float min_y = INFINITY, max_y = -INFINITY;\
	ui_element_list_node* n = o->child_list.head.next;\
	while(n != &o->child_list.tail){\
		if(n->data->prev_pos.y < min_y) min_y = n->data->prev_pos.y;\
		if(n->data->prev_pos.y + n->data->size.y > max_y) max_y = n->data->prev_pos.y + n->data->size.y;\
		n = n->next;\
	}\
	float box_h = max_y - min_y;\
	float lb_space_h = o->size.y - o->corner_size.y*2;\
	if(o->sb){\
		if(box_h > lb_space_h)\
			scrollbar_set_slider_max(o->sb, ceil((box_h - lb_space_h) * 10));\
		else\
			scrollbar_set_slider_max(o->sb, 0);\
	}\
	float box_off_y = o->sb ? o->sb->slider_pos * 0.1 : 0;\
	vec2f off = {o->pos.x + o->corner_size.x, o->pos.y + o->size.y - o->corner_size.y + box_off_y - max_y};\
	n = o->child_list.head.next;\
	while(n != &o->child_list.tail){\
		n->data->pos = vec2_add(n->data->prev_pos, off);\
		n->data->rerender = 1;/* changed before calling ui_element_render method of superclass, so child element will be re-rendered before being drawn*/\
		n = n->next;\
	}\
}

static void _listbox_render(ui_element* _o)
{
	listbox* o = (listbox*)_o;
	if(o->prev_scrollbar_pos != o->sb->slider_pos){
		o->prev_scrollbar_pos = o->sb->slider_pos;
		o->rerender = 1;
	}
	UI_ELEMENT_ON_RERENDER(RERENDER());
	render_obj_draw(&o->ro_atex);

	render_push_bbox(vec2_add(o->pos, o->corner_size), (vec2f){o->size.x - o->corner_size.x*2, o->size.y - o->corner_size.y*2});
	_ui_element_render(_o);
	render_pop_bbox();
}

void listbox_create(listbox* o, vec2f pos, vec2f size, vec2f corner_size,
			struct ui_full_box_textures box_tex)
{
	ui_element_create((ui_element*)o, pos, size);
	vt_init(o->vt, vt_insert(&vt, ui_element_render, _listbox_render));

	o->orig_size = size;
	o->corner_size = corner_size;
	o->box_tex = box_tex;
	o->sb = NULL;

	o->ro_atex = RENDER_OBJ_EMPTY;
	RERENDER();
}

void listbox_set_scrollbar(listbox* o, scrollbar* sb)
{
	o->size.x = o->orig_size.x - sb->size.x;
	sb->pos = (vec2f){ o->pos.x + o->size.x, o->pos.y };
	sb->size.y = o->size.y;
	sb->rerender = 1;

	o->sb = sb;
	o->rerender = 1;
}
void listbox_add_child(listbox* o, ui_element* child)
{
	ui_element_add_child((ui_element*)o, child);
	o->rerender = 1;
}
