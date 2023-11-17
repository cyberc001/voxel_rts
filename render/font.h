#ifndef RENDER_FONT_H
#define RENDER_FONT_H

#include "formats/font.h"
#include "render/base.h"
#include "more_math.h"

render_obj render_char(font* f, utf_char fcc, vec2f coords, vec2f size, vec3f clr);

// render_text() options
#define RENDER_TEXT_OPTIONS_END		0
#define RENDER_TEXT_WRAP		1		// int wrap_mode, double(float) wrap_w
	#define WRAP_CHAR			1
	#define WRAP_WORD			2
#define RENDER_TEXT_OUT_H		2		// float* out_h

render_obj render_text(font* f, const char* text, vec2f coords, vec2f size, vec3f clr, ...);

float text_width(font* f, const char* text, vec2f size);
float text_height(font* f, const char* text, vec2f size);

#endif
