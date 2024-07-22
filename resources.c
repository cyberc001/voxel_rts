#include "resources.h"
#include <stdio.h>
#include <dirent.h>

#include "log.h"
#include "formats/qb_vxl.h"

size_t str_hash(size_t table_sz, const char** key)
{
	const char* i= *key;
	size_t h = 0;
	for(; *i; ++i)
		h = ((h + *i) % table_sz * 256) % table_sz;
	return h;
}
int str_cmp(const char** s1, const char** s2)
{
	return strcmp(*s1, *s2);
}

/* TEXTURE ATLASES */
atlas_texture_map map_atlas_texture;
void atlas_texture_add(char* name, atlas_texture* tex)
{
	atlas_texture_map_insert(&map_atlas_texture, name, *tex);
}
atlas_texture* atlas_texture_find(const char* name)
{
	return atlas_texture_map_find(&map_atlas_texture, name);
}

/* SOUNDS */
sound_map map_sound;
void sound_add(char* name, sound snd)
{
	sound_map_insert(&map_sound, name, snd);
}
sound sound_find(const char* name)
{
	sound* s = sound_map_find(&map_sound, name);
	return s ? *s : NULL;
}

/* MODELS */
model_map map_model;
void model_add(char* name, voxel_model model)
{
	model_map_insert(&map_model, name, model);
}
voxel_model* model_find(const char* name)
{
	return model_map_find(&map_model, name);
}

/* FONTS */
font_map map_font;
void font_add(char* name, font f)
{
	font_map_insert(&map_font, name, f);
}
font* font_find(const char* name)
{
	return font_map_find(&map_font, name);
}

/* INITIALIZATION */
void resources_init()
{
	atlas_texture_map_create(&map_atlas_texture, 16, str_hash, str_cmp);
	sound_map_create(&map_sound, 16, str_hash, str_cmp);
	model_map_create(&map_model, 16, str_hash, str_cmp);
	font_map_create(&map_font, 16, str_hash, str_cmp);
}

#define CONCAT_ENT_NAME() (strcat(strcat(strcpy(malloc(strlen(dir_name) + strlen(ent->d_name) + 2), dir_name), "/"), ent->d_name))
#define STR_ENDS_WITH(str, sub) ((strlen(str) >= strlen(sub)) && (!memcmp(sub, str + strlen(str) - strlen(sub), strlen(sub))))

#define SCAN_TYPE_TEXTURE_ATLAS		0x1
#define SCAN_TYPE_SOUND			0x2
#define SCAN_TYPE_VOXEL			0x3
#define SCAN_TYPE_FONT			0x4

char* cut_fname_ext(const char* fname)
{
	char* name_dot = strrchr(fname, '.');
	return name_dot ? strndup(fname, strlen(fname) - strlen(name_dot)) : strdup(fname);
}
char* get_only_fname(const char* fname)
{
	const char* name_slash = strrchr(fname, '/');
	const char* name_dot = strrchr(fname, '.');
	size_t ln = strlen(fname);
	
	const char* name_start = name_slash ? name_slash + 1 : fname;
	const char* name_end = (name_dot && name_dot != fname) ? name_dot : fname + ln;
	return strndup(name_start, name_end - name_start);
}

static void r_resources_scan(const char* dir_name, int scan_type)
{
	DIR* dir;
	struct dirent* ent;
	if((dir = opendir(dir_name))){
		while((ent = readdir(dir))){
			if(ent->d_type == DT_DIR){
				if(!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
					continue;

				char* new_dir_name = CONCAT_ENT_NAME();
				r_resources_scan(new_dir_name, scan_type);
				free(new_dir_name);
			}
			else if(ent->d_type == DT_REG)
				switch(scan_type){
					case SCAN_TYPE_TEXTURE_ATLAS: {
						if(!STR_ENDS_WITH(ent->d_name, ".lon"))
							continue;
						char* fname = CONCAT_ENT_NAME();
						FILE* fd = fopen(fname, "r");
						if(fd){
							read_texture_atlas(fd);
							fclose(fd);
						}
						else
							LOG_ERROR("Cannot open file \"%s\" for reading", fname);
						free(fname);
					} break;
					case SCAN_TYPE_SOUND: {
						char* fname = CONCAT_ENT_NAME();
						sound snd = audio_load_sound(fname);
						if(snd)
							sound_add(cut_fname_ext(ent->d_name), snd);
						free(fname);
					} break;
					case SCAN_TYPE_VOXEL: {
						char* fname = CONCAT_ENT_NAME();
						FILE* fd = fopen(fname, "r");
						if(fd){
							voxel_array arr = read_qb_vxl(fd);
							for(size_t i = 0; i < arr.busy; ++i)
								if(arr.data[i].model.buf_sizes[0])
									model_add(cut_fname_ext(ent->d_name), arr.data[i]);
							voxel_array_destroy(&arr);
							fclose(fd);
						}
						else
							LOG_ERROR("Cannot open file \"%s\" for reading", fname);
						free(fname);
					} break;
					case SCAN_TYPE_FONT: {
						if(!STR_ENDS_WITH(ent->d_name, ".lon"))
							continue;
						char* fname = CONCAT_ENT_NAME();
						FILE* fd = fopen(fname, "r");
						if(fd){
							read_font_desc(fd);
							fclose(fd);
						}
						else
							LOG_ERROR("Cannot open file \"%s\" for reading", fname);
						free(fname);
					} break;
				}
		}
	}
	else
		LOG_ERROR("Cannot open directory \"%s\" for scanning resources", dir_name);
}
void resources_scan()
{
	// atlas textures
	DIR* dir;
	struct dirent* ent;
	if((dir = opendir("."))){
		while((ent = readdir(dir))){
			if(!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
				continue;
			if(ent->d_type == DT_DIR){
				if(!strcmp(ent->d_name, "texture_atlases")) r_resources_scan(ent->d_name, SCAN_TYPE_TEXTURE_ATLAS);
				else if(!strcmp(ent->d_name, "sounds")) r_resources_scan(ent->d_name, SCAN_TYPE_SOUND);
				else if(!strcmp(ent->d_name, "voxels")) r_resources_scan(ent->d_name, SCAN_TYPE_VOXEL);
				else if(!strcmp(ent->d_name, "fonts")) r_resources_scan(ent->d_name, SCAN_TYPE_FONT);
			}
		}
		closedir(dir);
	}
	else
		LOG_ERROR("Cannot open current directory for scanning resources");

	load_texture_atlases();
}
