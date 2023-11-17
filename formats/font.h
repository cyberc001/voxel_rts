#ifndef FORMAT_TTF_H
#define FORMAT_TTF_H

#include <GL/gl.h>
#include <stdio.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "more_math.h"
#include "utf.h"

typedef struct {
	utf_char begin, end;
} font_char_range;

typedef struct {
	float tex_coords[8];
	vec2f size; // bitmap size divided by font size - to relate to other characters in the same font
	vec2f bearing; // bitmap bearings divided by font size
	vec2f advance; // coordinate increment for rendering the next character in string
} font_char;

#include "htable_oa.h"
DEF_HTABLE_OA(font_char_map, utf_char, font_char);

typedef struct {
	font_char_map char_map;
	GLuint tex_id;
} font;

void read_font_desc(FILE* fd); // reads a font description file (in LON format) and adds resulting fonts to resources
font read_font(const char* fname, int size, font_char_range* char_ranges, size_t char_ranges_ln);

font_char* font_get_char(font* f, utf_char c);

#endif
