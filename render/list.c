#include "render/list.h"
#include <stdio.h>

#define RENDER_LIST_GROWTH 8

render_list *render_list_front, *render_list_mid, *render_list_back;

void render_list_init()
{
	render_list_front = render_list_create();
	render_list_mid = render_list_create();
	render_list_back = render_list_create();
}
render_list* render_list_create()
{
	render_list* list = malloc(sizeof(render_list));
	render_info_draw_dynarray_create(&list->draw);
	render_info_free_dynarray_create(&list->free);
	return list;
}
void render_list_reset(render_list* list)
{
	list->draw.busy = list->free.busy = 0;
}	

void render_list_add_draw(render_info_draw* inf)
{
	render_info_draw_dynarray_push(&render_list_back->draw, *inf);
}
void render_list_add_free(render_info_free* inf)
{
	render_info_free_dynarray_push(&render_list_back->free, *inf);
}


static int can_swap_mid_and_front = 0;
void render_list_swap_back_and_mid()
{
	render_list_reset(render_list_mid);
	swap(render_list_back, render_list_mid);
	can_swap_mid_and_front = 1;
}
void render_list_swap_front_and_mid()
{
	if(!can_swap_mid_and_front)
		return;
	can_swap_mid_and_front = 0;
	render_list_reset(render_list_front);
	swap(render_list_front, render_list_mid);
}
