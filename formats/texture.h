#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/gl.h>
#include <stdio.h>

typedef struct {
	void* data;
	unsigned width, height;
	GLenum clr_type;
	GLenum data_type;
} texture_info;

texture_info read_texture(FILE* fd);
GLuint load_texture_from_info(texture_info* info);
GLuint load_texture(FILE* fd);

#endif
