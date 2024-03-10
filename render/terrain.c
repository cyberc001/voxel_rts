#include "render/terrain.h"
#include "game/terrain.h"

#define MODEL_SET_DATA(x, y, z, tex, u, v){\
	model_vert[model_idx] = (x) * TERRAIN_PIECE_SIZE;\
	model_vert[model_idx + 1] = (y) * TERRAIN_PIECE_SIZE;\
	model_vert[model_idx + 2] = (z) * TERRAIN_PIECE_SIZE;\
	model_tex[model_idx / 3] = tex;\
	model_uv[model_idx / 3 * 2] = u;\
	model_uv[model_idx / 3 * 2 + 1] = v;\
	model_idx += 3;\
}

void render_terrain()
{
	for(size_t i = 0; i < chunks.size; ++i)
		if(chunk_dict_is_allocated(&chunks, i)){
			terrain_chunk* chnk = &chunks.data[i];
			if(chnk->flags & TERRAIN_CHUNK_FLAG_RENDER_CHANGED){
				render_obj_free(&chnk->render_data);

				size_t model_ln = 64 * 3;
				GLfloat* model_vert = malloc(model_ln * sizeof(GLfloat));
				GLuint* model_tex = malloc((model_ln / 3) * sizeof(GLuint));
				GLfloat* model_uv = malloc((model_ln / 3 * 2) * sizeof(GLfloat));
				size_t model_idx = 0;

				uint32_t chnk_x = KEY_COORD_X(chunks.keys[i]);
				uint32_t chnk_y = KEY_COORD_Y(chunks.keys[i]);
				for(size_t j = 0; j < TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_SIZE; ++j){
					terrain_piece* piece = chnk->data[j];
					uint32_t world_x = chnk_x * TERRAIN_CHUNK_SIZE + j % TERRAIN_CHUNK_SIZE;
					uint32_t world_y = chnk_y * TERRAIN_CHUNK_SIZE + j / TERRAIN_CHUNK_SIZE;
					while(piece){
						atlas_texture** atex = piece->atex;
						if(model_idx + 12*6 >= model_ln){
							model_ln = model_ln * 2 > model_idx + 12*6 ? model_ln * 2 : model_ln + 12*6;
							model_vert = realloc(model_vert, model_ln * sizeof(GLfloat));
							model_tex = realloc(model_tex, (model_ln / 3) * sizeof(GLuint));
							model_uv = realloc(model_uv, (model_ln / 3 * 2) * sizeof(GLfloat));
						}
						MODEL_SET_DATA(world_x, piece->z_floor[0], world_y, atex[0]->atlas_layer, atex[0]->coords[0], atex[0]->coords[1]);
						MODEL_SET_DATA(world_x + 1, piece->z_floor[1], world_y, atex[0]->atlas_layer, atex[0]->coords[2], atex[0]->coords[3]);
						MODEL_SET_DATA(world_x + 1, piece->z_floor[2], world_y + 1, atex[0]->atlas_layer, atex[0]->coords[4], atex[0]->coords[5]);
						MODEL_SET_DATA(world_x, piece->z_floor[3], world_y + 1, atex[0]->atlas_layer, atex[0]->coords[6], atex[0]->coords[7]);

						MODEL_SET_DATA(world_x, piece->z_floor[0], world_y, atex[1]->atlas_layer, atex[1]->coords[0], atex[1]->coords[1]);
						MODEL_SET_DATA(world_x + 1, piece->z_floor[1], world_y, atex[1]->atlas_layer, atex[1]->coords[2], atex[1]->coords[3]);
						MODEL_SET_DATA(world_x + 1, piece->z_ceil[1], world_y, atex[1]->atlas_layer, atex[1]->coords[4], atex[1]->coords[5]);
						MODEL_SET_DATA(world_x, piece->z_ceil[0], world_y, atex[1]->atlas_layer, atex[1]->coords[6], atex[1]->coords[7]);

						MODEL_SET_DATA(world_x, piece->z_floor[3], world_y + 1, atex[2]->atlas_layer, atex[2]->coords[0], atex[2]->coords[1]);
						MODEL_SET_DATA(world_x + 1, piece->z_floor[2], world_y + 1, atex[2]->atlas_layer, atex[2]->coords[2], atex[2]->coords[3]);
						MODEL_SET_DATA(world_x + 1, piece->z_ceil[2], world_y + 1, atex[2]->atlas_layer, atex[2]->coords[4], atex[2]->coords[5]);
						MODEL_SET_DATA(world_x, piece->z_ceil[3], world_y + 1, atex[2]->atlas_layer, atex[2]->coords[6], atex[2]->coords[7]);

						MODEL_SET_DATA(world_x, piece->z_floor[0], world_y, atex[3]->atlas_layer, atex[3]->coords[0], atex[3]->coords[1]);
						MODEL_SET_DATA(world_x, piece->z_floor[3], world_y + 1, atex[3]->atlas_layer, atex[3]->coords[2], atex[3]->coords[3]);
						MODEL_SET_DATA(world_x, piece->z_ceil[3], world_y + 1, atex[3]->atlas_layer, atex[3]->coords[4], atex[3]->coords[5]);
						MODEL_SET_DATA(world_x, piece->z_ceil[0], world_y, atex[3]->atlas_layer, atex[3]->coords[6], atex[3]->coords[7]);

						MODEL_SET_DATA(world_x + 1, piece->z_floor[1], world_y, atex[4]->atlas_layer, atex[4]->coords[0], atex[4]->coords[1]);
						MODEL_SET_DATA(world_x + 1, piece->z_floor[2], world_y + 1, atex[4]->atlas_layer, atex[4]->coords[2], atex[4]->coords[3]);
						MODEL_SET_DATA(world_x + 1, piece->z_ceil[2], world_y + 1, atex[4]->atlas_layer, atex[4]->coords[4], atex[4]->coords[5]);
						MODEL_SET_DATA(world_x + 1, piece->z_ceil[1], world_y, atex[4]->atlas_layer, atex[4]->coords[6], atex[4]->coords[7]);

						MODEL_SET_DATA(world_x, piece->z_ceil[0], world_y, atex[5]->atlas_layer, atex[5]->coords[0], atex[5]->coords[1]);
						MODEL_SET_DATA(world_x + 1, piece->z_ceil[1], world_y, atex[5]->atlas_layer, atex[5]->coords[2], atex[5]->coords[3]);
						MODEL_SET_DATA(world_x + 1, piece->z_ceil[2], world_y + 1, atex[5]->atlas_layer, atex[5]->coords[4], atex[5]->coords[5]);
						MODEL_SET_DATA(world_x, piece->z_ceil[3], world_y + 1, atex[5]->atlas_layer, atex[5]->coords[6], atex[5]->coords[7]);

						piece = piece->next;
					}
				}

				model_ln = model_idx;
				model_vert = realloc(model_vert, model_ln * sizeof(GLfloat));
				model_tex = realloc(model_tex, (model_ln / 3) * sizeof(GLuint));
				model_uv = realloc(model_uv, (model_ln / 3 * 2) * sizeof(GLfloat));

				chnk->render_data = render_obj_create(GL_QUADS, 0, model_vert, model_ln * sizeof(GLfloat), RENDER_OBJ_TEXTURE, model_tex, (model_ln / 3) * sizeof(GLuint), RENDER_OBJ_TEX_COORDS, model_uv, (model_ln / 3 * 2) * sizeof(GLfloat), RENDER_OBJ_ATTRIBUTES_END);

				free(model_vert); free(model_tex); free(model_uv);
			}
			render_obj_draw(&chnk->render_data);
		}
}
