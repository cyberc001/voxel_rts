#include "formats/texture_atlas.h"
#include "formats/lon.h"
#include "formats/texture.h"
#include "resources.h"
#include "render/shader.h"

#include "log.h"

#define GET_TEXTURE_PARAM(param, desc)\
	lon_object** _tex_info_ ## param = lon_dict_find(tex_info, #param);\
	if(_tex_info_ ## param){\
		lon_object* tex_info_  ## param = *_tex_info_ ## param;\
		switch(tex_info_ ## param ->type){\
			case LON_OBJECT_TYPE_INTEGER:\
				param = ((lon_integer*)tex_info_ ## param)->value;\
				break;\
			case LON_OBJECT_TYPE_FLOAT:\
				param = ((lon_float*)tex_info_ ## param)->value;\
				break;\
			default:\
				LOG_ERROR("Texture info #%lu for atlas #%lu does have " desc " of type %s instead of INTEGER or FLOAT",  j, i, lon_type_to_string(tex_info_ ## param));\
				continue;\
		}\
	}

static texture_info* _texture_data = NULL;
static size_t _texture_data_sz = 0;

void read_texture_atlas(FILE* fd)
{
	lon_array* arr = read_lon(fd);
	for(size_t i = 0; i + 1 < arr->size; i += 2){
		if(arr->data[i]->type != LON_OBJECT_TYPE_STRING){
			LOG_ERROR("Atlas texture name is not STRING, but %s", lon_type_to_string(arr->data[i]));
			continue;
		}
		if(arr->data[i + 1]->type != LON_OBJECT_TYPE_ARRAY){
			LOG_ERROR("Atlas textures are not ARRAY, but %s", lon_type_to_string(arr->data[i + 1]));
			continue;
		}

		lon_string* tex_file = (lon_string*)arr->data[i];
		FILE* tex_fd = fopen(tex_file->value, "rb");
		if(!tex_fd){
			LOG_ERROR("Cannot open texture file \"%s\" for reading", tex_file->value);
			continue;
		}
		texture_info tex = read_texture(tex_fd);
		if(!tex.data){
			LOG_ERROR("File format for texture file \"%s\" is not supported, recognized or valid", tex_file->value);
			continue;
		}

		lon_array* textures = (lon_array*)arr->data[i + 1];
		/* current texture coordinates for auto-filling them in */
		double x = 0, y = 0;
		double w = 0, h = 0;

		for(size_t j = 0; j < textures->size; ++j){
			lon_dict* tex_info = (lon_dict*)textures->data[j];
			if(tex_info->type != LON_OBJECT_TYPE_DICT){
				LOG_ERROR("Texture info #%lu for atlas #%lu is not DICT, but %s",  j, i, lon_type_to_string((lon_object*)tex_info));
				continue;
			}

			lon_string** _tex_id = (lon_string**)lon_dict_find(tex_info, "id");
			if(!_tex_id){
				LOG_ERROR("Texture info #%lu for atlas #%lu doesn't have an ID",  j, i);
				continue;
			}
			lon_string* tex_id = *_tex_id;
			if(tex_id->type != LON_OBJECT_TYPE_STRING){
				LOG_ERROR("Texture info #%lu for atlas #%lu does have an ID of type %s instead of STRING",  j, i, lon_type_to_string((lon_object*)tex_id));
				continue;
			}
 
			GET_TEXTURE_PARAM(x, "X coordinate")
			GET_TEXTURE_PARAM(y, "Y coordinate")
			GET_TEXTURE_PARAM(w, "width")
			GET_TEXTURE_PARAM(h, "height")

			if(w <= 0) LOG_WARN("Texture info #%lu for atlas #%lu has width of %g\n", j, i, w);
			if(h <= 0) LOG_WARN("Texture info #%lu for atlas #%lu has width of %g\n", j, i, h);

			atlas_texture atex = { _texture_data_sz + 1, {0} };
			atex.coords[0] = x; atex.coords[1] = y + h;
			atex.coords[2] = x + w; atex.coords[3] = y + h;
			atex.coords[4] = x + w; atex.coords[5] = y;
			atex.coords[6] = x; atex.coords[7] = y;
			atlas_texture_add(strdup(tex_id->value), &atex);

			x += w;
			if(x >= tex.width){
				x = 0;
				y += h;
			}
		}
		_texture_data = realloc(_texture_data, (++_texture_data_sz) * sizeof(texture_info));
		_texture_data[_texture_data_sz - 1] = tex;
	}
	lon_object_free((lon_object*)arr);
}

void load_texture_atlases()
{
	unsigned max_w = 0, max_h = 0;
	for(size_t i = 0; i < _texture_data_sz; ++i){
		texture_info* ti = _texture_data + i;
		if(ti->width > max_w)
			max_w = ti->width;
		if(ti->height > max_h)
			max_h = ti->height;
	}

	GLuint atlas_id;
	glGenTextures(1, &atlas_id);
	glActiveTexture(SHADER_ATLAS_TEXTURE);
	glBindTexture(GL_TEXTURE_2D_ARRAY, atlas_id);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, max_w, max_h, _texture_data_sz, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	for(size_t i = 0; i < _texture_data_sz; ++i){
		texture_info* ti = _texture_data + i;
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
					0, 0, i,
					ti->width, ti->height, 1,
					ti->clr_type, ti->data_type, ti->data);
		free(ti->data);
	}
	free(_texture_data);
	_texture_data = NULL;
	_texture_data_sz = 0;

	for(size_t i = 0; i < map_atlas_texture.size; ++i)
		if(atlas_texture_map_is_allocated(&map_atlas_texture, i)){
			atlas_texture* tex = map_atlas_texture.data + i;
			for(size_t xi = 0; xi < 8; xi += 2)
				tex->coords[xi] /= max_w;
			for(size_t yi = 1; yi < 8; yi += 2)
				tex->coords[yi] /= max_h;
		}

	shader_set_atlas_textures(atlas_id);
}
