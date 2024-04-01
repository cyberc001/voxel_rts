#include "ui/elements/textbox.h"

#define RERENDER_ALL(){\
	RERENDER_TEXT(); RERENDER_ATEX();\
}
#define RERENDER_TEXT(){\
	char* text = strdup(o->text);\
	o->space_w = text_width(o->fnt, "_", o->font_size);\
	o->text_w = text_width(o->fnt, text, o->font_size) + o->space_w;\
	int alflags = UI_ALIGN_CENTER_V | (o->text_w >= o->size.x ? UI_ALIGN_RIGHT : 0);\
	render_obj_free(&o->ro_text); render_obj_free(&o->ro_cursor);\
	o->ro_text = ui_render_aligned_text((ui_element*)o, o->fnt, text, o->font_size, o->text_clr, alflags);\
	o->ro_cursor = ui_render_aligned_text((ui_element*)o, o->fnt, "|", o->font_size, o->text_clr, UI_ALIGN_CENTER_V);\
	free(text);\
}
#define RERENDER_ATEX(){\
	render_obj_free(&o->ro_atex);\
	o->ro_atex = ui_render_lrm_box((ui_element*)o, o->box_tex);\
}

static void _textbox_render(ui_element* _o)
{
	textbox* o = (textbox*)_o;
	UI_ELEMENT_ON_RERENDER(RERENDER_ALL());
	render_obj_draw(&o->ro_atex);
	render_push_bbox(o->pos, o->size);

	if(o->text_w >= o->size.x){
		ui_offset(-o->space_w, 0);
		render_obj_draw(&o->ro_text);
		ui_offset(o->space_w, 0);
	}
	else
		render_obj_draw(&o->ro_text);

	float off_x = min(o->text_w - text_width(o->fnt, "_", o->font_size), o->size.x - text_width(o->fnt, "|", o->font_size) * 4);
	ui_offset(off_x, 0);
	render_obj_draw(&o->ro_cursor);
	ui_offset(-off_x, 0);
	render_pop_bbox();
}
static void _textbox_key_down(ui_element* _o, key_code key, int mods)
{
	textbox* o = (textbox*)_o;
	if(!o->focused)
		return;
	if(key == KEY_BACKSPACE && strlen(o->text)){
		// determine the length in bytes of erased codepoint
		const char *it = o->text, *prev_it = NULL;
		while(1){
			utf_next_char(&it);
			if(!*it)
				break;
			prev_it = it;
		}
		if(it > prev_it && prev_it)
			o->text[strlen(o->text) - (it - prev_it)] = '\0';
		else
			o->text[0] = '\0';
		o->rerender = 1;
	}
}
static void _textbox_key_char(ui_element* _o, utf_char c)
{
	textbox* o = (textbox*)_o;
	if(!o->focused)
		return;
	char out[9]; utf_encode_char(c, out);
	o->text = str_append(o->text, &o->text_ln, out);
	o->rerender = 1;
}

void textbox_create(textbox* o, vec2f pos, vec2f size, vec2f font_size,
			struct ui_lrm_box_textures box_tex,
			font* fnt)
{
	ui_element_create((ui_element*)o, pos, size);
	vt_init(o->vt, vt_insert(&vt, ui_element_render, _textbox_render);
			vt_insert(&vt, ui_element_key_down, _textbox_key_down);
			vt_insert(&vt, ui_element_key_char, _textbox_key_char));

	o->font_size = font_size;
	o->fnt = fnt;
	o->text_ln = 32; o->text = malloc(o->text_ln + 1);
	o->text[0] = '\0';
	o->text_clr = (vec3f){0.5, 0, 0};

	o->box_tex = box_tex;

	o->ro_text = o->ro_cursor = o->ro_atex = RENDER_OBJ_EMPTY;
	RERENDER_ALL();
}
