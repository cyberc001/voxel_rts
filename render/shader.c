#include "render/shader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMPILE_GL_SHADER(name, type, source)\
{\
	name = glCreateShader(type);\
	fprintf(stderr, "Compiling shader \"%s\"...\n", #name);\
	glShaderSource((name), 1, &(source), NULL);\
	glCompileShader(name);\
	GLint log_ln;\
	glGetShaderiv((name), GL_INFO_LOG_LENGTH, &log_ln);\
	GLchar* log = malloc(log_ln + 1);\
	glGetShaderInfoLog((name), log_ln, &log_ln, log);\
	log[log_ln] = '\0';\
	fprintf(stderr, "%s\n", log);\
	free(log);\
}

/* Shaders */
GLuint base_vsh, base_fsh;
GLuint base_shprog;
/* Shader resources */
static GLint base_vsh_proj_mat, base_vsh_mview_mat,
	     base_vsh_colorize;
/* Uniforms */
static GLuint atlas_texture = 0;

void shader_init()
{
	const char* base_vsh_src =
	"#version 450 core\n"
	"layout (location = 0) in vec3 a_pos;\n"
	"layout (location = 1) in vec3 a_clr;\n"
	"layout (location = 2) in uint a_atlas_layer;\n"
	"layout (location = 3) in vec2 a_tex_coords;\n"
	"out vec4 clr;\n"
	"out float atlas_layer;\n"
	"out vec2 tex_coords;\n"
	"uniform mat4 projection_mat;\n"
	"uniform mat4 modelview_mat;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	gl_Position = projection_mat * (modelview_mat * vec4(a_pos, 1));\n"
	"	clr = vec4(a_clr, 1);\n"
	"	atlas_layer = a_atlas_layer;\n"
	"	tex_coords = a_tex_coords;\n"
	"}";
	COMPILE_GL_SHADER(base_vsh, GL_VERTEX_SHADER, base_vsh_src);
	const char* base_fsh_src = 
	"#version 450 core\n"
	"in vec4 clr;\n"
	"in float atlas_layer;\n"
	"in vec2 tex_coords;\n"
	"\n"
	"uniform sampler2DArray atlas_texture;\n"
	"uniform sampler2D main_texture;\n"
	"uniform vec3 colorize = vec3(-1, -1, -1);\n"
	"layout (location = 0) out vec4 out_clr;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	if(atlas_layer >= 0.5)\n"
	"		out_clr = texture(atlas_texture, vec3(tex_coords.x, tex_coords.y, atlas_layer - 1));\n"
	"	else if(textureSize(main_texture, 0).x != 1)\n"
	"		out_clr = texture(main_texture, vec2(tex_coords.x, tex_coords.y));\n"
	"	else\n"
	"		out_clr = clr;\n"
	"	if(colorize.r >= 0)\n"
	"		out_clr = vec4(colorize, out_clr.r);\n"
	"}";
	COMPILE_GL_SHADER(base_fsh, GL_FRAGMENT_SHADER, base_fsh_src);

	base_shprog = glCreateProgram();
	glAttachShader(base_shprog, base_vsh);
	glAttachShader(base_shprog, base_fsh);
	glLinkProgram(base_shprog);
	glUseProgram(base_shprog);

	base_vsh_proj_mat = glGetUniformLocation(base_shprog, "projection_mat");
	base_vsh_mview_mat = glGetUniformLocation(base_shprog, "modelview_mat");
	base_vsh_colorize = glGetUniformLocation(base_shprog, "colorize");

	glUniform1i(glGetUniformLocation(base_shprog, "main_texture"), 1);
}

void shader_load_gl_matrix()
{
	GLfloat projection_mat[16];
	GLfloat modelview_mat[16];
	glGetFloatv(GL_PROJECTION_MATRIX, projection_mat);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_mat);
	glUniformMatrix4fv(base_vsh_proj_mat, 1, 0, projection_mat);
	glUniformMatrix4fv(base_vsh_mview_mat, 1, 0, modelview_mat);
}
void shader_set_atlas_textures(GLuint tex)
{
	if(atlas_texture)
		glDeleteTextures(1, &atlas_texture);
	atlas_texture = tex;
}

void shader_set_colorize(vec3f clr)
{
	float out[3] = {clr.x, clr.y, clr.z};
	glUniform3fv(base_vsh_colorize, 1, out);
}
void shader_clear_colorize()
{
	float out[3] = {-1, -1, -1};
	glUniform3fv(base_vsh_colorize, 1, out);
}

