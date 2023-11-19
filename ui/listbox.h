#ifndef LISTBOX_H
#define LISTBOX_H

#include "ui/ui_element.h"
#include "render/texture.h"
#include "ui/scrollbar.h"

typedef struct listbox listbox;
struct listbox {
	struct ui_element;

	vec2f orig_size;
	vec2f corner_size;
	struct ui_full_box_textures box_tex;

	scrollbar* sb; // Scrollbar is separate from the element and can be absent
	int prev_scrollbar_pos;

	render_obj ro_atex;
};

void listbox_crerate(listbox* o, vec2f pos, vec2f size, vec2f corner_size,
			struct ui_full_box_textures box_tex);

void listbox_set_scrollbar(listbox* o, scrollbar* sb); // Shrinks listbox to fit the scrollbar (with scrollbar's original width) on the right, sets scrollbar's position to the right of listbox

#endif
