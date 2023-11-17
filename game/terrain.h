#ifndef GAME_TERRAIN_H
#define GAME_TERRAIN_H

#include <stdint.h>
#include "htable_oa.h"
#include "game/types.h"
#include "render/base.h"
#include "resources.h"

#define COORD_KEY(x, y) (((uint64_t)(x) << 32) | (y))
#define KEY_COORD_X(key) ((key) >> 32)
#define KEY_COORD_Y(key) ((key) & 0xFFFFFFFF)

typedef struct terrain_piece terrain_piece;
struct terrain_piece {
	terrain_piece *prev, *next; // there is always a dummy head for the double-linked list to save cpu cycles on allocation of TERRAIN_CHUNK_SIZE^2 pointers per chunk.
	float z_floor[4], z_ceil[4]; // 0: (x, y) 1: (x+1, y) 2: (x+1, y+1) 3: (x, y+1)

	atlas_texture* atex[6]; // each side can have a unique texture

	enum armor_type armor;
	double health;
};


#define TERRAIN_CHUNK_SIZE 16

#define TERRAIN_CHUNK_FLAG_RENDER_CHANGED	0x1

typedef struct {
	terrain_piece* data; // 2d array of size TERRAIN_CHUNK_SIZE x TERRAIN_CHUNK_SIZE
	render_obj render_data;
	int flags;
} terrain_chunk;

DEF_HTABLE_OA(chunk_dict, uint64_t, terrain_chunk)
extern chunk_dict chunks;
void terrain_init();

terrain_chunk* terrain_get_chunk(uint32_t x, uint32_t y);
terrain_chunk* terrain_get_chunk_anyway(uint32_t x, uint32_t y); // creates a chunk for these coordinates if it doesn't exist
terrain_chunk* terrain_create_chunk(uint32_t x, uint32_t y);
void terrain_mark_changed_chunk(uint32_t x, uint32_t y); // sets the flag to re-render the chunk

terrain_piece* terrain_get_chunk_piece(terrain_chunk* chnk, uint32_t x, uint32_t y); // in chunk coordinates, not world coodinates
terrain_piece* terrain_get_piece(uint32_t x, uint32_t y);
terrain_piece* terrain_get_piece_anyway(uint32_t x, uint32_t y); // use it also for creating terrain pieces
void terrain_mark_changed_piece(uint32_t x, uint32_t y); // same as terrain_mark_changed_chunk(), just calculates chunk coordinates for you

void terrain_piece_add(terrain_piece* list, terrain_piece* toadd);
void terrain_piece_remove(terrain_piece* toremove);

#endif
