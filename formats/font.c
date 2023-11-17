#include "formats/font.h"
#include "log.h"
#include "render/shader.h"
#include "formats/lon.h"
#include "resources.h"

#define XOR_SWAP(x, y)\
{\
	(x) = (x) ^ (y);\
	(y) = (y) ^ (x);\
	(x) = (x) ^ (y);\
}

static int ttf_init = 0;
static int desc_init = 0;
static FT_Library ttf_lib;

static size_t __hash_uint(size_t table_sz, const unsigned* key)
{
	return (*key) % table_sz;
}

typedef struct {
	font_char_range* ranges;
	size_t ranges_sz;
} font_char_range_array;

DEF_HTABLE_OA(char_range_map, char*, font_char_range_array)
char_range_map map_char_range;

#define CREATE_CHAR_RANGE_ARRAY(name, sz) char_range_map_insert(&map_char_range, #name, (font_char_range_array){malloc(sizeof(font_char_range) * sz), sz})

void read_font_desc(FILE* fd)
{
	if(!desc_init){
		char_range_map_create(&map_char_range, 128, str_hash, str_cmp);
		font_char_range_array* basic = CREATE_CHAR_RANGE_ARRAY(basic, 1);
		basic->ranges[0] = (font_char_range){0x10, 0x7E};
		font_char_range_array* russian = CREATE_CHAR_RANGE_ARRAY(russian, 3);
		russian->ranges[0] = (font_char_range){0x410, 0x44F};
		russian->ranges[1] = (font_char_range){0x401, 0x401}; // Ё
		russian->ranges[2] = (font_char_range){0x451, 0x451}; // ё

		desc_init = 1;
	}

	lon_array* arr = read_lon(fd);
	for(size_t i = 0; i + 1 < arr->size; i += 2){
		if(arr->data[i]->type != LON_OBJECT_TYPE_STRING){
			LOG_ERROR("Font name is not STRING, but %s", lon_type_to_string(arr->data[i]));
			continue;
		}
		if(arr->data[i + 1]->type != LON_OBJECT_TYPE_DICT){
			LOG_ERROR("Font data is not DICT, but %s", lon_type_to_string(arr->data[i + 1]));
			continue;
		}

		lon_string* font_file = (lon_string*)arr->data[i];
		lon_dict* font_data = (lon_dict*)arr->data[i + 1];

		lon_object** size_ptr = lon_dict_find(font_data, "size");
		if(size_ptr && (*size_ptr)->type != LON_OBJECT_TYPE_INTEGER){
			LOG_WARN("Size for font %s is not INTEGER, but %s", font_file->value, lon_type_to_string(*size_ptr));
			size_ptr = NULL;
		}
		int size = size_ptr ? ((lon_integer*)*size_ptr)->value : 32;

		lon_object** char_range_arr_ptr = lon_dict_find(font_data, "chars");
		if(char_range_arr_ptr && (*char_range_arr_ptr)->type != LON_OBJECT_TYPE_ARRAY){
			LOG_ERROR("Char range array for font %s is not ARRAY, but %s", font_file->value, lon_type_to_string(*char_range_arr_ptr));
			continue;
		}

		if(!char_range_arr_ptr){
			LOG_ERROR("No char range array for font %s. Skipping", font_file->value);
			continue;
		}

		lon_array* char_range_arr = (lon_array*)(*char_range_arr_ptr);
		if(!char_range_arr->size){
			LOG_ERROR("Char range array for font %s is empty", font_file->value);
			continue;
		}

		size_t ranges_sz = 0;
		font_char_range* ranges = NULL;

		for(size_t j = 0; j < char_range_arr->size; ++j){
			if(char_range_arr->data[j]->type != LON_OBJECT_TYPE_STRING){
				LOG_WARN("Char range #%lu for font %s is not STRING, but %s. Skipping", j, font_file->value, lon_type_to_string(char_range_arr->data[i]));
				continue;
			}

			font_char_range_array* crarr = char_range_map_find(&map_char_range, ((lon_string*)char_range_arr->data[j])->value);
			if(!crarr){
				LOG_WARN("Char range #%lu for font %s has unknown name \"%s\"", j, font_file->value, ((lon_string*)char_range_arr->data[j])->value);
				continue;
			}

			ranges_sz += crarr->ranges_sz;
			ranges = realloc(ranges, sizeof(font_char_range) * ranges_sz);
			for(size_t k = 0; k < crarr->ranges_sz; ++k)
				ranges[ranges_sz - crarr->ranges_sz + k] = crarr->ranges[k];
		}

		font f = read_font(font_file->value, size, ranges, ranges_sz);
		if(f.tex_id){
			char* f_name = get_only_fname(font_file->value);
			font_add(f_name, f);
		}
		free(ranges);
	}
	lon_object_free((lon_object*)arr);
}

#define CHARS_PER_ROW	100

font read_font(const char* fname, int size, font_char_range* char_ranges, size_t char_ranges_ln)
{
	font f = {.tex_id = 0};

	int err;
	if(!ttf_init){
		if(err = FT_Init_FreeType(&ttf_lib)){
			LOG_ERROR("Could not initalize FreeType library: error code %d", err);
			return f;
		}
		ttf_init = 1;
	}

	FT_Face face;
	err = FT_New_Face(ttf_lib, fname, 0, &face);
	if(err){
		LOG_ERROR("Could not create a FreeType face from file \"%s\": error code %d", fname, err);
		return f;
	}

	font_char_map_create(&f.char_map, 128, __hash_uint, NULL);

	FT_Set_Pixel_Sizes(face, 0, size);

	size_t char_count = 128;
	unsigned max_w = 0, max_h = 1,
			cur_w = 1, cur_char_count = 0;

	for(size_t i = 0; i < char_ranges_ln; ++i){
		for(utf_char c = char_ranges[i].begin; c <= char_ranges[i].end; ++c){
			if(err = FT_Load_Char(face, c, FT_LOAD_RENDER)){
				LOG_ERROR("Could not render character 0x%02X for FreeType font %s, size %d", c, fname, size);
				goto failure_cleanup;
			}
	
			cur_w += face->glyph->bitmap.width + 1;
			if(++cur_char_count >= CHARS_PER_ROW){
				if(cur_w > max_w)
					max_w = cur_w;
				cur_w = 1; cur_char_count = 0;
			}
			if(face->glyph->bitmap.rows > max_h)
				max_h = face->glyph->bitmap.rows + 1;
		}
	}

	// make the font atlas texture as square as possible
	unsigned tex_w = max_w, tex_h = (max_h + 1) * ((char_count + CHARS_PER_ROW - 1) / CHARS_PER_ROW);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // tell OpenGL that this texture is not 4-byte aligned
	glGenTextures(1, &f.tex_id);
	glActiveTexture(SHADER_MAIN_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, f.tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
			tex_w, tex_h,
			0, GL_RED, GL_UNSIGNED_BYTE,
			NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned x = 0, y = 0;

	for(size_t i = 0; i < char_ranges_ln; ++i){
		for(utf_char c = char_ranges[i].begin; c <= char_ranges[i].end; ++c){
			if(err = FT_Load_Char(face, c, FT_LOAD_RENDER)){
				LOG_ERROR("Could not render character 0x%02X for FreeType font %s, size %d", c, fname, size);
				goto failure_cleanup;
			}
	
			x += face->glyph->bitmap.width + 1;
			if(x >= max_w){
				x = face->glyph->bitmap.width + 1;
				y += max_h + 1;
			}
	
			// mirror bitmap vertically to be properly (and easily) displayable in OpenGL (it's worth it)
			for(size_t _y = 0; _y < face->glyph->bitmap.rows / 2; ++_y)
				for(size_t _x = 0; _x < face->glyph->bitmap.width; ++_x)
					XOR_SWAP(face->glyph->bitmap.buffer[_x + _y * face->glyph->bitmap.width], face->glyph->bitmap.buffer[_x + (face->glyph->bitmap.rows - _y - 1) * face->glyph->bitmap.width]);
	
			glTexSubImage2D(GL_TEXTURE_2D, 0,
					x - face->glyph->bitmap.width - 1, y,
					face->glyph->bitmap.width, face->glyph->bitmap.rows,
					GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
	
			font_char ch = {
				.size = {(float)face->glyph->bitmap.width / size, (float)face->glyph->bitmap.rows / size},
				.bearing = {(float)face->glyph->bitmap_left / size, (float)face->glyph->bitmap_top / size},
				.advance = {(float)(face->glyph->advance.x >> 6) / size, -1}
			};
			ch.tex_coords[0] = (float)(x - face->glyph->bitmap.width - 1) / tex_w; ch.tex_coords[1] = (float)y / tex_h;
			ch.tex_coords[2] = (float)(x - 1) / tex_w; ch.tex_coords[3] = (float)y / tex_h;
			ch.tex_coords[4] = (float)(x - 1) / tex_w; ch.tex_coords[5] = (float)(y + face->glyph->bitmap.rows) / tex_h;
			ch.tex_coords[6] = (float)(x - 1 - face->glyph->bitmap.width) / tex_w; ch.tex_coords[7] = (float)(y + face->glyph->bitmap.rows) / tex_h;
	
			font_char_map_insert(&f.char_map, c, ch);
		}
	}

cleanup:
	FT_Done_Face(face);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	return f;
failure_cleanup:
	glDeleteTextures(1, &f.tex_id); f.tex_id = 0;
	goto cleanup; // god WHY
}

font_char* font_get_char(font* f, utf_char c)
{
	return font_char_map_find(&f->char_map, c);
}
