#ifndef ITEMBOX_H
#define ITEMBOX_H

#include "ui/ui_element.h"
#include "render/texture.h"
#include "ui/elements/scrollbar.h"
#include "dyn_array.h"

typedef struct {
	char* str;
	render_obj ro_text, ro_atex;
	render_obj ro_text_s, ro_atex_s;
} itembox_item;
DEF_DYN_ARRAY(itembox_arr, itembox_item)

typedef struct itembox itembox;
struct itembox {
	struct ui_element;

	vec2f orig_size;
	vec2f corner_size;

	vec2f text_size; vec3f text_clr; vec3f text_select_clr; font* text_fnt;
	float item_h;

	struct ui_full_box_textures box_tex;
	struct ui_lrm_box_textures item_tex, item_tex_s;

	size_t selection;

	scrollbar* sb; // Scrollbar is separate from the element and can be absent

	itembox_arr items;

	render_obj ro_atex;
};

void itembox_create(itembox* o, vec2f pos, vec2f size, vec2f corner_size,
			vec2f text_size, font* text_fnt, float item_h, 
			struct ui_full_box_textures box_tex,
			struct ui_lrm_box_textures item_tex, struct ui_lrm_box_textures item_tex_s);

void itembox_set_scrollbar(itembox* o, scrollbar* sb); // Shrinks itembox to fit the scrollbar (with scrollbar's original width) on the right, sets scrollbar's position to the right of itembox

size_t itembox_add_item(itembox* o, char* item);
void itembox_delete_item(itembox* o, size_t i);
char* itembox_get_selected_item(itembox* o);

#endif
