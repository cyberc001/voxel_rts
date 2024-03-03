#include "game/terrain.h"
#include <stdio.h>

chunk_dict chunks;

static size_t chunk_hash(size_t table_sz, const uint64_t* key)
{
	// hash table size will (should) not exceed uint32_t anyway, so only 16 lowest bits from each coordinate are used for hash
	return (*key & 0xFFFF | ((*key & 0xFFFF00000000) >> 16));
}

float tpiece_max_z_ceil(terrain_piece* tpiece)
{
	float _max = tpiece->z_ceil[0];
	for(size_t i = 1; i < 4; ++i)
		if(tpiece->z_ceil[i] > _max)
			_max = tpiece->z_ceil[i];
	return _max;
}

#define TEST_ADD_TPIECE()\
	terrain_piece* add2 = malloc(sizeof(terrain_piece));\
	add2->z_floor[0] = add2->z_floor[1] = add2->z_floor[2] = add2->z_floor[3] = 1;\
	for(size_t j = 0; j < 6; ++j)\
		add2->atex[j] = atlas_texture_find("gravel");\

void terrain_init()
{
	chunk_dict_create(&chunks, 16, chunk_hash);
	for(size_t x = 0; x < 10; ++x)
		for(size_t y = 0; y < 10; ++y){
			terrain_piece* p = terrain_get_piece_anyway(x, y);

			/* TEST */
			if(x >= 2 && x <= 4 && y == 2){
				TEST_ADD_TPIECE()
				add2->z_floor[0] = add2->z_floor[1] = add2->z_floor[2] = add2->z_floor[3] = 1 + (x - 2);
				add2->z_ceil[0] = 1 + (x - 2);
				add2->z_ceil[1] = 2 + (x - 2);
				add2->z_ceil[2] = 2 + (x - 2);
				add2->z_ceil[3] = 1 + (x - 2);
				terrain_piece_add(p, add2);
			}
			if(x == 5 && y == 2){
				TEST_ADD_TPIECE()
				add2->z_floor[0] = add2->z_floor[1] = add2->z_floor[2] = add2->z_floor[3] = 3;
				add2->z_ceil[0] = 4;
				add2->z_ceil[1] = 4;
				add2->z_ceil[2] = 4;
				add2->z_ceil[3] = 4;
				terrain_piece_add(p, add2);
			}
			if(x == 5 && y >= 3 && y <= 5){
				TEST_ADD_TPIECE()
				add2->z_floor[0] = add2->z_floor[1] = add2->z_floor[2] = add2->z_floor[3] = 4 + (y - 3);
				add2->z_ceil[0] = 4 + (y - 3);
				add2->z_ceil[1] = 4 + (y - 3);
				add2->z_ceil[2] = 5 + (y - 3);
				add2->z_ceil[3] = 5 + (y - 3);
				terrain_piece_add(p, add2);
			}
			if(x == 5 && y == 6){
				TEST_ADD_TPIECE()
				add2->z_floor[0] = add2->z_floor[1] = add2->z_floor[2] = add2->z_floor[3] = 6;
				add2->z_ceil[0] = 7;
				add2->z_ceil[1] = 7;
				add2->z_ceil[2] = 7;
				add2->z_ceil[3] = 7;
				terrain_piece_add(p, add2);
			}
			if(x >= 2 && x <= 4 && y == 6){
				TEST_ADD_TPIECE()
				add2->z_floor[0] = add2->z_floor[1] = add2->z_floor[2] = add2->z_floor[3] = 7 + (4 - x);
				add2->z_ceil[0] = 8 + (4 - x);
				add2->z_ceil[1] = 7 + (4 - x);
				add2->z_ceil[2] = 7 + (4 - x);
				add2->z_ceil[3] = 8 + (4 - x);
				terrain_piece_add(p, add2);
			}
			if(x == 1 && y == 6){
				TEST_ADD_TPIECE()
				add2->z_floor[0] = add2->z_floor[1] = add2->z_floor[2] = add2->z_floor[3] = 9;
				add2->z_ceil[0] = 10;
				add2->z_ceil[1] = 10;
				add2->z_ceil[2] = 10;
				add2->z_ceil[3] = 10;
				terrain_piece_add(p, add2);
			}
			/* TEST END */

			terrain_piece* add = malloc(sizeof(terrain_piece));

			add->z_floor[0] = add->z_floor[1] = add->z_floor[2] = add->z_floor[3] = 0;
			add->z_ceil[0] = 1;
			add->z_ceil[1] = 1;
			add->z_ceil[2] = 1;
			add->z_ceil[3] = 1;

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
