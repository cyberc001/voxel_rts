#ifndef RESOURCES_H
#define RESOURCES_H

#include <GL/gl.h>
#include "htable_oa.h"
#include "audio.h"
#include "render/base.h"
#include "formats/font.h"
#include "formats/qb_vxl.h"

/* Helper functions */
size_t str_hash(size_t table_sz, const char** key);
int str_cmp(const char** s1, const char** s2);

char* cut_fname_ext(const char* fname);
char* get_only_fname(const char* fname);

/* Initializes hash tables for every resource type. */
void resources_init();
/* Scans pre-determined directories for resource files. */
void resources_scan();

/* TEXTURE ATLASES */
typedef struct {
	GLuint atlas_layer; // layer number in texture atlas (texture array) [1 IS SUBTRACTED FROM IT IN shader.c SO 0 MEANS NO TEXTURES]
	GLfloat coords[8];
} atlas_texture;
DEF_HTABLE_OA(atlas_texture_map, char*, atlas_texture)
extern atlas_texture_map map_atlas_texture;

void atlas_texture_add(char* name, atlas_texture* tex);
atlas_texture* atlas_texture_find(const char* name);

/* SOUNDS */
DEF_HTABLE_OA(sound_map, char*, sound)
extern sound_map map_sound;

void sound_add(char* name, sound snd);
sound sound_find(const char* name);

/* MODELS */
DEF_HTABLE_OA(model_map, char*, voxel_model)
extern model_map map_model;

void model_add(char* name, voxel_model model);
voxel_model* model_find(const char* name);

/* FONTS */
DEF_HTABLE_OA(font_map, char*, font)
extern font_map map_font;

void font_add(char* name, font f);
font* font_find(const char* name);

#endif
