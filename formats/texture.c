#include "formats/texture.h"

#include "png.h"
#include <stdlib.h>

static GLenum clr_type_png_to_gl(int clr_type)
{
	switch(clr_type)
	{
		case PNG_COLOR_TYPE_RGB: return GL_RGB;
		case PNG_COLOR_TYPE_RGBA: return GL_RGBA;
		default: return GL_RGB;
	}
}

texture_info read_texture(FILE* fd)
{
	unsigned char sig[8];
	fread(sig, 1, 8, fd);

	if(png_check_sig(sig, 8)){ // PNG file
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if(!png_ptr)
			return (texture_info){.data = NULL};
		png_infop info_ptr = png_create_info_struct(png_ptr);
		if(!info_ptr){
			png_destroy_read_struct(&png_ptr, NULL, NULL);
			return (texture_info){.data = NULL};
		}

		if(setjmp(png_jmpbuf(png_ptr))){
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return (texture_info){.data = NULL};
		}

		png_init_io(png_ptr, fd);
		png_set_sig_bytes(png_ptr, 8);

		png_uint_32 width, height;
		int bit_depth, clr_type;
		png_read_info(png_ptr, info_ptr);
		png_get_IHDR(png_ptr, info_ptr,
				&width, &height, &bit_depth, &clr_type,
				NULL, NULL, NULL);

		if(clr_type == PNG_COLOR_TYPE_PALETTE)
			png_set_expand(png_ptr);
		if(clr_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
			png_set_expand(png_ptr);
		if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			png_set_expand(png_ptr);
		if(bit_depth == 16)
			png_set_strip_16(png_ptr);
		if(clr_type == PNG_COLOR_TYPE_GRAY || clr_type == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(png_ptr);
		png_read_update_info(png_ptr, info_ptr);

		png_uint_32 rowbytes = png_get_rowbytes(png_ptr, info_ptr);

		png_byte* data = malloc(rowbytes * height * sizeof(png_byte));
		png_bytepp row_pointers = malloc(height * sizeof(png_bytep));
		for(png_uint_32 i = 0; i < height; ++i)
			row_pointers[i] = data + i * rowbytes;

		png_read_image(png_ptr, row_pointers);

		free(row_pointers);
		png_read_end(png_ptr, NULL);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

		return (texture_info){
			.data = data,
			.width = width, .height = height,
			.clr_type = clr_type_png_to_gl(clr_type),
			.data_type = GL_UNSIGNED_BYTE
		};
	}

	return (texture_info){.data = NULL};
}

GLuint load_texture_from_info(texture_info* info)
{
	if(!info->data)
		return 0;

	GLuint tex_id;
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, info->width, info->height, 0, info->clr_type, info->data_type, info->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	free(info->data);
	return tex_id;
}

GLuint load_texture(FILE* fd)
{
	texture_info inf = read_texture(fd);
	return load_texture_from_info(&inf);
}
