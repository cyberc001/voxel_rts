#ifndef SHADER_H
#define SHADER_H

#include <GL/gl.h>
#include <stddef.h>
#include "more_math.h"

extern GLuint base_vsh, base_fsh;
extern GLuint base_shprog;

void shader_init();

void shader_load_gl_matrix();

/* These textures are set once and should not be changed: */
#define SHADER_ATLAS_TEXTURE	GL_TEXTURE0
#define SHADER_MAIN_TEXTURE	GL_TEXTURE1
void shader_set_atlas_textures(GLuint tex);

void shader_set_colorize(vec3f clr);
void shader_clear_colorize();

#endif
