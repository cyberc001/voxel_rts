#include "ui.h"
#include "controls.h"

static ui_element_list _list;

static ui_element* focus = NULL;

static void ui_key_down(key_code key, int mods)
{
	ui_element_list_node* n = _list.head.next;
	vec2f mc = get_mouse_coords();
	while(n != &_list.tail){
		if(is_point_in_rect2f(mc, (rect2f){n->data->pos, n->data->size}) && key_code_is_mouse(key)){
			if(focus)
				focus->focused = 0;
			n->data->focused = 1;
			focus = n->data;
		}
		vt_call(*(n->data), ui_element_key_down, key, mods);
		n = n->next;
	}
}
static void ui_key_up(key_code key, int mods)
{
	ui_element_list_node* n = _list.head.next;
	while(n != &_list.tail){
		vt_call(*(n->data), ui_element_key_up, key, mods);
		n = n->next;
	}
}
static void ui_key_char(utf_char c)
{
	ui_element_list_node* n = _list.head.next;
	while(n != &_list.tail){
		vt_call(*(n->data), ui_element_key_char, c);
		n = n->next;
	}
}
static void ui_mouse_move(float x, float y)
{
	ui_element_list_node* n = _list.head.next;
	while(n != &_list.tail){
		vt_call(*(n->data), ui_element_mouse_move, x, y);
		n = n->next;
	}
}

void ui_init()
{
	ui_element_list_create(&_list);
	controls_add_key_down(ui_key_down);
	controls_add_key_up(ui_key_up);
	controls_add_key_char(ui_key_char);
	controls_add_mouse_move(ui_mouse_move);
}
void ui_render()
{
	ui_element_list_node* n = _list.head.next;
	while(n != &_list.tail){
		vt_call(*(n->data), ui_element_render);
		n = n->next;
	}
}

void ui_add_element(ui_element* ue)
{
	ue->list_ptr = ui_element_list_insert_end(&_list, ue);
}
