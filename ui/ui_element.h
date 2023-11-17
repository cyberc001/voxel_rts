#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include "object.h"
#include "more_math.h"
#include "controls.h"
#include "render/texture.h"
#include "linked_list.h"

typedef struct ui_element ui_element;
DEF_LINKED_LIST(ui_element_list, ui_element*)
struct ui_element {
	struct object;

	void* list_ptr; // pointer to a ui_element_list node, defined in render/ui.c, used for deleting the element from the list of ui elements
	vec2f pos, size,
	      prev_pos; // useful mostly for managing child elements

	int rerender; // whether to re-create some render objects this frame or not
	int focused;

	ui_element* parent;
	ui_element_list child_list;
	ui_element* child_focused;
};

void ui_element_create(ui_element* o, vec2f pos, vec2f size);
void ui_element_add_child(ui_element* o, ui_element* child);

typedef void (*ui_element_render)(ui_element* o);
typedef void (*ui_element_key_down)(ui_element* o, key_code key, int mods);
typedef void (*ui_element_key_up)(ui_element* o, key_code key, int mods);
typedef void (*ui_element_key_char)(ui_element* o, utf_char c);
typedef void (*ui_element_mouse_move)(ui_element* o, float x, float y);

void _ui_element_render(ui_element* o);
void _ui_element_key_down(ui_element* o, key_code key, int mods);
void _ui_element_key_up(ui_element* o, key_code key, int mods);
void _ui_element_key_char(ui_element* o, utf_char c);
void _ui_element_mouse_move(ui_element* o, float x, float y);

#define UI_ELEMENT_ON_RERENDER(...)\
	if(o->rerender){\
		__VA_ARGS__;\
		o->rerender = 0;\
	}

/* Common functions */
render_obj ui_render_lrm_box(ui_element* o, atlas_texture* l, atlas_texture* r, atlas_texture* m); // Render a box consisting from left, right and middle textures
render_obj ui_render_udm_box(ui_element* o, atlas_texture* u, atlas_texture* d, atlas_texture* m); // Render a box consisting from up, down and middle textures
render_obj ui_render_full_box(ui_element* o, vec2f corner_size,
			atlas_texture* lt, atlas_texture* rt, atlas_texture* lb, atlas_texture* rb,
			atlas_texture* u, atlas_texture* d, atlas_texture* l, atlas_texture* r,
			atlas_texture* m);

#define UI_ALIGN_CENTER_V	0x1
#define UI_ALIGN_CENTER_H	0x2
#define UI_ALIGN_CENTER_BOTH	(UI_ALIGN_CENTER_V | UI_ALIGN_CENTER_H)
#define UI_ALIGN_RIGHT		0x4
render_obj ui_render_aligned_text(ui_element* o, font* fnt, const char* text, vec2f font_size, vec3f clr, int flags); // center/right-left/top-bottom flags are meant to be mutually exclusive with each other
void ui_offset(float x, float y);

#endif
