#include "render/font.h"
#include <stdlib.h>
#include <ctype.h>

#define MODEL_SET_DATA(x, y, z, u, v)\
{\
	verts[model_idx] = x;\
	verts[model_idx + 1] = y;\
	verts[model_idx + 2] = z;\
	tex_coords[(model_idx / 3 * 2)] = u;\
	tex_coords[(model_idx / 3 * 2) + 1] = v;\
	model_idx += 3;\
}

render_obj render_char(font* f, utf_char fcc, vec2f coords, vec2f size, vec3f clr)
{
	font_char* fc = font_get_char(f,fcc);
	if(!fc)
		return (render_obj){.flags = RENDER_OBJ_FLAG_INVALID};
	TRANSLATE_ORTHO_COORDS(coords);
	coords.x += fc->bearing.x * size.x * 2;
	coords.y -= size.y * (fc->size.y - fc->bearing.y) * 2;
	size.x *= fc->size.x; size.y *= fc->size.y;
	TRANSLATE_ORTHO_SIZE(size);

	GLfloat verts[] = {coords.x,coords.y,0, coords.x+size.x,coords.y,0, coords.x+size.x,coords.y+size.y,0, coords.x,coords.y+size.y,0};

	return render_obj_create(GL_QUADS, 0, verts, sizeof(verts), RENDER_OBJ_TEX_COORDS, fc->tex_coords, sizeof(fc->tex_coords), RENDER_OBJ_MAIN_TEXTURE, f->tex_id, RENDER_OBJ_COLORIZE, clr, RENDER_OBJ_ATTRIBUTES_END);
}

render_obj render_text(font* f, const char* text, vec2f coords, vec2f size, vec3f clr, ...)
{
	va_list args;
	va_start(args, clr);
	struct {
		// real options
		int wrap; float wrap_w;
		float* out_h;

		// variables used in algorithms
		size_t* word_wraps; size_t word_wraps_sz;
		float min_y, max_y;
	} options = {
		.wrap = 0, .out_h = NULL
	};

	int opt;
	while((opt = va_arg(args, int)) != RENDER_TEXT_OPTIONS_END)
		switch(opt){
			case RENDER_TEXT_WRAP:
				options.wrap = va_arg(args, int);
				options.wrap_w = va_arg(args, double);
				break;
			case RENDER_TEXT_OUT_H:
				options.out_h = va_arg(args, float*);
				options.min_y = INFINITY;
				options.max_y = -INFINITY;
				break;
		}

	size_t model_ln = 3 * 16;
	size_t model_idx = 0;
	GLfloat* verts = malloc(model_ln * sizeof(GLfloat));
	GLfloat* tex_coords = malloc((model_ln / 3 * 2) * sizeof(GLfloat));

	TRANSLATE_ORTHO_COORDS(coords);
	TRANSLATE_ORTHO_SIZE(size);

	if(options.wrap == WRAP_WORD){
		options.word_wraps = NULL; options.word_wraps_sz = 0;
	
		vec2f cur_coords = coords;
		float word_beg_x, word_end_x = 0;
		size_t word_beg_i;
		int line_beg = 1;

		utf_char c, prev_c = ' ';
		size_t i = 0;
		const char* text_pt = text;
		while(c = utf_next_char(&text_pt)){
			font_char* fc = font_get_char(f, c);
			if(!fc)
				continue;

			if(line_beg){
				if(fc->size.x <= 0 || fc->size.y <= 0) { ++i; continue; }// skip whitespace in the beginning of the line when wrapping
				else line_beg = 0;
			}

			if(isspace(prev_c) && !isspace(c)){ // word begins
				word_beg_x = word_end_x = cur_coords.x;
				word_beg_i = i;
			}
			if(!isspace(prev_c) && isspace(c)){ // word ends
				if(word_end_x >= coords.x + options.wrap_w*2){
					cur_coords.x = coords.x + (word_end_x - word_beg_x);
					word_beg_x = word_end_x = cur_coords.x - 1;
					options.word_wraps = realloc(options.word_wraps, sizeof(size_t) * (++options.word_wraps_sz));
					options.word_wraps[options.word_wraps_sz - 1] = word_beg_i + 1;
					line_beg = 1;
				}
			}

			vec2f char_coords = cur_coords;
			char_coords.x += fc->bearing.x * size.x;
			if(!isspace(c))
				word_end_x = char_coords.x + size.x*fc->size.x;
			cur_coords.x += fc->advance.x*size.x;
			++i; prev_c = c;
		}
		if(word_end_x >= coords.x + options.wrap_w*2){
			cur_coords.x = coords.x + (word_end_x - word_beg_x);
			options.word_wraps = realloc(options.word_wraps, sizeof(size_t) * (++options.word_wraps_sz));
			options.word_wraps[options.word_wraps_sz - 1] = word_beg_i;
		}


		options.word_wraps = realloc(options.word_wraps, sizeof(size_t) * (++options.word_wraps_sz));
		options.word_wraps[options.word_wraps_sz - 1] = (size_t)-1;
	}

	vec2f cur_coords = coords;
	utf_char c; size_t i = 0, word_wraps_i = 0;
	int line_beg = 1; // beginning of a new line; used to skip whitespaces when wrrapping
	while(c = utf_next_char(&text)){
		if(model_idx + 3 * 4 >= model_ln){
			model_ln *= 2;
			verts = realloc(verts, model_ln * sizeof(GLfloat));
			tex_coords = realloc(tex_coords, (model_ln / 3 * 2) * sizeof(GLfloat));
		}
		font_char* fc = font_get_char(f, c);
		if(!fc)
			continue;

		switch(options.wrap){
			case WRAP_CHAR:
				if(cur_coords.x + fc->bearing.x*size.x + size.x*fc->size.x >= coords.x + options.wrap_w*2){
					cur_coords.x = coords.x - fc->bearing.x*size.x;
					cur_coords.y += fc->advance.y*size.y;
					line_beg = 1;
				}
				break;
			case WRAP_WORD:
				++i;
				if(options.word_wraps[word_wraps_i] == i){
					cur_coords.x = coords.x - fc->bearing.x*size.x;
					cur_coords.y += fc->advance.y*size.y;
					++word_wraps_i;
					line_beg = 1;
				}
				break;
		}

		if(options.wrap && line_beg){
			if(fc->size.x <= 0 || fc->size.y <= 0) continue; // skip whitespace in the beginning of the line when wrapping
			else line_beg = 0;
		}

		vec2f char_coords = cur_coords;
		if(options.out_h)
			if(char_coords.y + size.y*fc->size.y > options.max_y)
				options.max_y = char_coords.y + size.y*fc->size.y;
		char_coords.x += fc->bearing.x*size.x;
		char_coords.y -= size.y * (fc->size.y - fc->bearing.y);
		if(options.out_h)
			if(char_coords.y < options.min_y)
				options.min_y = char_coords.y;

		MODEL_SET_DATA(char_coords.x, char_coords.y, 0, fc->tex_coords[0], fc->tex_coords[1]);
		MODEL_SET_DATA(char_coords.x + size.x*fc->size.x, char_coords.y, 0, fc->tex_coords[2], fc->tex_coords[3]);
		MODEL_SET_DATA(char_coords.x + size.x*fc->size.x, char_coords.y + size.y*fc->size.y, 0, fc->tex_coords[4], fc->tex_coords[5]);
		MODEL_SET_DATA(char_coords.x, char_coords.y + size.y*fc->size.y, 0, fc->tex_coords[6], fc->tex_coords[7]);

		cur_coords.x += fc->advance.x*size.x;
	}

	verts = realloc(verts, model_idx * sizeof(GLfloat));
	tex_coords = realloc(tex_coords, (model_idx / 3 * 2) * sizeof(GLfloat));

	render_obj robj = render_obj_create(GL_QUADS, 0, verts, model_idx * sizeof(GLfloat), RENDER_OBJ_TEX_COORDS, tex_coords, sizeof(GLfloat) * (model_idx / 3 * 2), RENDER_OBJ_MAIN_TEXTURE, f->tex_id, RENDER_OBJ_COLORIZE, clr, RENDER_OBJ_ATTRIBUTES_END);

	free(verts);
	free(tex_coords);
	if(options.word_wraps)
		free(options.word_wraps);
	if(options.out_h)
		*options.out_h = options.max_y - options.min_y;

	return robj;
}

float text_width(font* f, const char* text, vec2f size)
{
	vec2f cur_coords = {0, 0};
	utf_char c;
	float last_bearing = 0;
	while(c = utf_next_char(&text)){
		font_char* fc = font_get_char(f, c);
		if(!fc)
			continue;
		vec2f char_coords = cur_coords;
		char_coords.x += fc->bearing.x*size.x;
		cur_coords.x += fc->advance.x*size.x;

		last_bearing = fc->size.x*size.x - fc->advance.x*size.x;
	}
	return cur_coords.x + last_bearing;
}
float text_height(font* f, const char* text, vec2f size)
{
	vec2f cur_coords = {0, 0};
	utf_char c;
	float h = 0;
	while(c = utf_next_char(&text)){
		font_char* fc = font_get_char(f, c);
		if(!fc)
			continue;
		vec2f char_coords = cur_coords;
		char_coords.y += size.y * fc->bearing.y;
		if(char_coords.y > h)
			h = char_coords.y;
	}
	return h;
}
