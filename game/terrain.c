#include "game/terrain.h"
#include <stdio.h>

chunk_dict chunks;

static size_t chunk_hash(size_t table_sz, const uint64_t* key)
{
	// hash table size will (should) not exceed uint32_t anyway, so only 16 lowest bits from each coordinate are used for hash
	return (*key & 0xFFFF | ((*key & 0xFFFF00000000) >> 16));
}

void terrain_init()
{
	chunk_dict_create(&chunks, 16, chunk_hash);
	for(size_t x = 0; x < 10; ++x)
		for(size_t y = 0; y < 10; ++y){
			terrain_piece* p = terrain_get_piece_anyway(x, y);

			terrain_piece* add = malloc(sizeof(terrain_piece));
			add->z_floor[0] = add->z_floor[1] = add->z_floor[2] = add->z_floor[3] = 0;
			add->z_ceil[0] = 1;
			add->z_ceil[1] = 1;
			add->z_ceil[2] = 1;
			add->z_ceil[3] = 1;

			if(y >= 2 && y <= 9 && x == 5){
				add->z_ceil[0] = 3;
				add->z_ceil[1] = 3;
				add->z_ceil[2] = 3;
				add->z_ceil[3] = 3;
			}

			if(y >= 0 && y <= 4 && x == 3){
				add->z_ceil[0] = 3;
				add->z_ceil[1] = 3;
				add->z_ceil[2] = 3;
				add->z_ceil[3] = 3;
			}
			/*if(y >= 0 && y <= 10 && x == 3){
				add->z_ceil[0] = 3;
				add->z_ceil[1] = 3;
				add->z_ceil[2] = 3;
				add->z_ceil[3] = 3;
			}

			if(x == 2 && y == 4){
				add->z_ceil[0] = 1;
				add->z_ceil[1] = 3;
				add->z_ceil[2] = 3;
				add->z_ceil[3] = 1;
			}
			if(x == 4 && y == 8){
				add->z_ceil[0] = 3;
				add->z_ceil[1] = 1;
				add->z_ceil[2] = 1;
				add->z_ceil[3] = 3;
			}*/

			for(size_t j = 1; j < 5; ++j)
				add->atex[j] = atlas_texture_find("grass_side");
			add->atex[5] = atlas_texture_find("grass_top");
			add->atex[0] = atlas_texture_find("dirt");

			terrain_piece_add(p, add);
			terrain_mark_changed_piece(x, y);
		}
}

terrain_chunk* terrain_get_chunk(uint32_t x, uint32_t y)
{
	return chunk_dict_find(&chunks, COORD_KEY(x, y));
}
terrain_chunk* terrain_get_chunk_anyway(uint32_t x, uint32_t y)
{
	terrain_chunk* chnk = chunk_dict_find(&chunks, COORD_KEY(x, y));
	if(chnk)
		return chnk;
	else
		return terrain_create_chunk(x, y);
}
terrain_chunk* terrain_create_chunk(uint32_t x, uint32_t y)
{
	terrain_chunk chnk = {.data = memset(malloc(sizeof(terrain_piece) * TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_SIZE), 0, sizeof(terrain_piece) * TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_SIZE), .flags = TERRAIN_CHUNK_FLAG_RENDER_CHANGED};
	return chunk_dict_insert(&chunks, COORD_KEY(x, y), chnk);
}
void terrain_mark_changed_chunk(uint32_t x, uint32_t y)
{
	terrain_chunk* chnk = terrain_get_chunk(x, y);
	if(chnk)
		chnk->flags |= TERRAIN_CHUNK_FLAG_RENDER_CHANGED;
}

terrain_piece* terrain_get_chunk_piece(terrain_chunk* chnk, uint32_t x, uint32_t y)
{
	return &chnk->data[y * TERRAIN_CHUNK_SIZE + x];
}
terrain_piece* terrain_get_piece(uint32_t x, uint32_t y)
{
	terrain_chunk* chnk = terrain_get_chunk(x / TERRAIN_CHUNK_SIZE, y / TERRAIN_CHUNK_SIZE);
	if(chnk)
		return terrain_get_chunk_piece(chnk, x % TERRAIN_CHUNK_SIZE, y % TERRAIN_CHUNK_SIZE);
	return NULL;
}
terrain_piece* terrain_get_piece_anyway(uint32_t x, uint32_t y)
{
	terrain_chunk* chnk = terrain_get_chunk(x / TERRAIN_CHUNK_SIZE, y / TERRAIN_CHUNK_SIZE);
	if(chnk)
		return terrain_get_chunk_piece(chnk, x % TERRAIN_CHUNK_SIZE, y % TERRAIN_CHUNK_SIZE);
	return terrain_get_chunk_piece(terrain_create_chunk(x / TERRAIN_CHUNK_SIZE, y / TERRAIN_CHUNK_SIZE), x % TERRAIN_CHUNK_SIZE, y % TERRAIN_CHUNK_SIZE);
}
void terrain_mark_changed_piece(uint32_t x, uint32_t y)
{
	terrain_mark_changed_chunk(x / TERRAIN_CHUNK_SIZE, y / TERRAIN_CHUNK_SIZE);
}

void terrain_piece_add(terrain_piece* list, terrain_piece* toadd)
{
	// adding to the beginning of the list
	if(list->next)
		list->next->prev = toadd;
	toadd->next = list->next; toadd->prev = list;
	list->next = toadd;
}
void terrain_piece_remove(terrain_piece* toremove)
{
	if(toremove->prev)
		toremove->prev->next = toremove->next;
	if(toremove->next)
		toremove->next->prev = toremove->prev;
}
