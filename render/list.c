#include "render/list.h"
#include <stdio.h>

#define RENDER_LIST_EMPTY ((render_list){.ln = 0, .data = NULL})
#define RENDER_LIST_GROWTH 8

static size_t render_list_capacity = 0;
render_list *render_list_front, *render_list_mid, *render_list_back;

void render_list_init()
{
	render_list_front = malloc(sizeof(render_list));
	render_list_mid = malloc(sizeof(render_list));
	render_list_back = malloc(sizeof(render_list));
	*render_list_front = *render_list_mid = *render_list_back = RENDER_LIST_EMPTY;
}
void render_list_add(const render_info* inf)
{
	if(render_list_back->ln >= render_list_capacity){
		render_list_capacity += RENDER_LIST_GROWTH;
		render_list_front->data = realloc(render_list_front->data, render_list_capacity * sizeof(render_info));
		render_list_mid->data = realloc(render_list_mid->data, render_list_capacity * sizeof(render_info));
		render_list_back->data = realloc(render_list_back->data, render_list_capacity * sizeof(render_info));
	}
	render_list_back->data[render_list_back->ln++] = *inf;
}

static int can_swap_mid_and_front = 0;
void render_list_swap_back_and_mid()
{
	render_list_mid->ln = 0;
	swap(render_list_back, render_list_mid);
	can_swap_mid_and_front = 1;
}
void render_list_swap_front_and_mid()
{
	if(!can_swap_mid_and_front)
		return;
	can_swap_mid_and_front = 0;
	render_list_front->ln = 0;
	swap(render_list_front, render_list_mid);
}
