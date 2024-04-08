#include "game/terrain.h"
#include "game/pathfinding.h"
#include <stdio.h>

chunk_dict chunks;

static size_t chunk_hash(size_t table_sz, const uint64_t* key)
{
	// hash table size will (should) not exceed uint32_t anyway, so only 16 lowest bits from each coordinate are used for hash
	return (*key & 0xFFFF | ((*key & 0xFFFF00000000) >> 16));
}

#define TEST_ADD_TPIECE()\
	terrain_piece* add2 = malloc(sizeof(terrain_piece));\
	add2->occupying_objects = 0;\
	add2->z_floor[0] = add2->z_floor[1] = add2->z_floor[2] = add2->z_floor[3] = 1;\
	for(size_t j = 0; j < 6; ++j)\
		add2->atex[j] = atlas_texture_find("gravel");\

void terrain_init()
{
	chunk_dict_create(&chunks, 16, chunk_hash);
	for(size_t x = 0; x < 10; ++x)
		for(size_t y = 0; y < 10; ++y){
			terrain_piece* p = terrain_get_piece_anyway(x, y);

			p->occupying_objects = 0;
			p->z_floor[0] = p->z_floor[1] = p->z_floor[2] = p->z_floor[3] = 0;
			p->z_ceil[0] = 1;
			p->z_ceil[1] = 1;
			p->z_ceil[2] = 1;
			p->z_ceil[3] = 1;

			for(size_t j = 1; j < 5; ++j)
				p->atex[j] = atlas_texture_find("grass_side");
			p->atex[5] = atlas_texture_find("grass_top");
			p->atex[0] = atlas_texture_find("dirt");

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
	terrain_chunk chnk = {.data = memset(malloc(sizeof(terrain_piece*) * TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_SIZE), 0, sizeof(terrain_piece*) * TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_SIZE), .flags = TERRAIN_CHUNK_FLAG_RENDER_CHANGED};
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
	return chnk->data[y * TERRAIN_CHUNK_SIZE + x];
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
	terrain_piece* tpiece = NULL;
	if(!chnk)
		chnk = terrain_create_chunk(x / TERRAIN_CHUNK_SIZE, y / TERRAIN_CHUNK_SIZE);
	tpiece = terrain_get_chunk_piece(chnk, x % TERRAIN_CHUNK_SIZE, y % TERRAIN_CHUNK_SIZE);
	if(tpiece)
		return tpiece;

	tpiece = malloc(sizeof(terrain_piece));
	tpiece->prev = NULL;
	tpiece->next = NULL;
	chnk->data[y * TERRAIN_CHUNK_SIZE + x] = tpiece;
	return tpiece;
}
void terrain_mark_changed_piece(uint32_t x, uint32_t y)
{
	terrain_mark_changed_chunk(x / TERRAIN_CHUNK_SIZE, y / TERRAIN_CHUNK_SIZE);
}

float tpiece_max_z_ceil(terrain_piece* tpiece)
{
	float _max = tpiece->z_ceil[0];
	for(size_t i = 1; i < 4; ++i)
		if(tpiece->z_ceil[i] > _max)
			_max = tpiece->z_ceil[i];
	return _max;
}
terrain_piece* terrain_get_nearest_piece(float z, terrain_piece* tpiece)
{
	float min_z = INFINITY;
	terrain_piece* nearest_tpiece = NULL;
	while(tpiece){
		float avg_z = tpiece_avg_z_ceil(*tpiece); 
		if(avg_z <= z && avg_z < min_z){
			min_z = avg_z;
			nearest_tpiece = tpiece;
		}
		tpiece = tpiece->next;
	}
	return nearest_tpiece;
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

void terrain_occupy_hexahedron(const hexahedron* h, int occupy)
{
	vec3f center = hexahedron_get_center(h);
	bbox3f h_bbox = hexahedron_get_bbox(h);
	h_bbox.min = vec3_sub(h_bbox.min, center);
	h_bbox.max = vec3_sub(h_bbox.max, center);
	float h_bbox_y = h_bbox.min.y;
	h_bbox.min.y -= h_bbox_y; h_bbox.max.y -= h_bbox_y;
	h_bbox.min.x += 0.5; h_bbox.min.z += 0.5;
	h_bbox.max.x += 0.5; h_bbox.max.z += 0.5;

	vec2i cur_pos = (vec2i){center.x, center.z};
	terrain_piece* cur_tpiece = terrain_get_piece(cur_pos.x, cur_pos.y);
	if(!cur_tpiece) return;
	cur_tpiece = terrain_get_nearest_piece(center.y, cur_tpiece);

	int bbox_w = ceil(h_bbox.max.x - h_bbox.min.x),
	    bbox_h = ceil(h_bbox.max.z - h_bbox.min.z);
	if(bbox_w % 2 == 0) ++bbox_w;
	if(bbox_h % 2 == 0) ++bbox_h;

	int buf_ln = bbox_w*bbox_h;
	terrain_piece** tpiece_buf = malloc(sizeof(terrain_piece*)*buf_ln);
	memset(tpiece_buf, 0, buf_ln * sizeof(terrain_piece*));
	tnode_dynarray node_buf;
	tnode_dynarray_create(&node_buf);
	get_all_occupied_tpieces(bbox_w, bbox_h,
					cur_pos, cur_tpiece,
					tpiece_buf, &node_buf);

	for(int i = 0; i < buf_ln; ++i)
		if(tpiece_buf[i]){
			if(occupy) 	terrain_occupy_piece(*tpiece_buf[i]);
			else		terrain_deoccupy_piece(*tpiece_buf[i]);
		}

	free(tpiece_buf);
	tnode_dynarray_destroy(&node_buf);
}

terrain_piece* __check_against_line(vec2f v, line3f* l, int x_changed)
{
	vec3f dir = vec3_sub(l->p2, l->p1);
	dir = vec3_norm(dir);
	float x = floor(v.x), z = floor(v.y);

	terrain_piece* tpiece = terrain_get_piece(x, z);
	while(tpiece){
		hexahedron h = hexahedron_from_terrain_piece(x, z, tpiece);

		for(size_t _f = 0; _f < 6; ++_f){
			vec3f e1 = vec3_sub(h.f[_f].p[1], h.f[_f].p[0]), e2 = vec3_sub(h.f[_f].p[2], h.f[_f].p[1]);
			vec3f intersect;
			float fac = line_plane_intersect(*l, vec3_cross(e1, e2), h.f[_f].p[0], &intersect, NULL);
			if(fac >= 0 && fac <= 1 && !isnan(intersect.x) && point_is_in_face3(intersect, &h.f[_f]))
				return tpiece;
		}
		tpiece = tpiece->next;
	}

	return NULL;
}
#define CHECK_AGAINST_LINE(x, z, l, x_changed){\
	terrain_piece* tpiece = __check_against_line((vec2f){x, z}, l, x_changed);\
	if(tpiece){\
		if(pos) *pos = (vec3f){floor(x) + 0.5, tpiece_avg_z_ceil(*tpiece), floor(z) + 0.5};\
		return (tpiece);\
	}\
}
terrain_piece* terrain_find_first_piece_in_line(line3f l, vec3f* pos)
{
	float x0 = l.p1.x, z0 = l.p1.z;
	float x1 = l.p2.x, z1 = l.p2.z;
	// Find all terrain piece candidates using slightly modified Bresenham's line algorithm
	float dx = fabs(x1 - x0), dz = -fabs(z1 - z0);
	float sx = (x0 < x1 ? 1 : -1), sz = (z0 < z1 ? 1 : -1);
	float error = dx + dz;

	CHECK_AGAINST_LINE(x0, z0, &l, 0);
	for(;;){
		if(round(x0) == round(x1) && round(z0) == round(z1))
			break;
		float e2 = 2*error;
		if(e2 >= dz){
			if(round(x0) == round(x1))
				break;
			error += dz;
			x0 += sx;
			CHECK_AGAINST_LINE(x0, z0, &l, 1);
		}
		if(e2 <= dx){
			if(round(z0) == round(z1))
				break;
			error += dx;
			z0 += sz;
			CHECK_AGAINST_LINE(x0, z0, &l, 0);
		}
	}

	return NULL;
}
