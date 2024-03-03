#include "game/pathfinding.h"
#include "htable_oa.h"
#include "pqueue.h"
#include "dyn_array.h"

typedef struct tnode tnode;
struct tnode {
	vec2i pos; float y;
	terrain_piece* tpiece;
	float cost, heuristic;
	struct tnode* parent;
};

static float heuristic(vec2i v1, vec2i v2)
{
	vec2i dist = vec2_sub(v1, v2);
	return dist.x*dist.x + dist.y*dist.y; // avoid square root - doesn't matter when comparing distances
}
#define tnode_calc_heuristic(t, goal) ( (t).heuristic = heuristic((t).pos, (goal)) )

DEF_PHTABLE_OA(tptr_set, terrain_piece*, tnode*)
size_t tptr_hash(size_t table_sz, const terrain_piece** key)
{
	return (uintptr_t)(*key) % table_sz;
}
DEF_PQUEUE(tnode_pqueue, tnode*)
int tnode_cmp(const tnode** t1, const tnode** t2)
{
	if((*t1)->cost + (*t1)->heuristic < (*t2)->cost + (*t2)->heuristic) return -1;
	return (*t1)->cost + (*t1)->heuristic > (*t2)->cost + (*t2)->heuristic;
}
DEF_DYN_ARRAY(tnode_dynarray, tnode)

static terrain_piece* get_nearest_tpiece(float z, terrain_piece* tpiece)
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

#define DIFF_MORE(a, b) (fabs((a) - (b)) > 0.1)

#define CHECK_NEXT_TPIECE_HEIGHT(){\
	if(isnan(next_tpiece_z))\
		next_tpiece_z = next_z;\
	else if(DIFF_MORE(next_z, next_tpiece_z)){\
		continue;\
	}\
}
#define PUSH_SUCCESSOR(dx, dy){\
	printf("trying to push successor %d %d\n", _cur_node->pos.x + dx, _cur_node->pos.y + dy);\
	int do_move = 1;\
	float next_tpiece_z = NAN;\
	for(int i = 0; i < buf_ln; ++i){\
		if(!tpiece_buf[i])\
			continue;\
		tnode cur_node_base;\
		cur_node_base.tpiece = tpiece_buf[i];\
		cur_node_base.pos = (vec2i){_cur_node->pos.x + i % bbox_w - bbox_w/2, _cur_node->pos.y + i / bbox_w - bbox_h/2};\
		printf("going from pos %d %d\n", cur_node_base.pos.x, cur_node_base.pos.y);\
		tnode* cur_node = &cur_node_base;\
		get_successor(dx, dy, cur_node, &node_buf);\
		\
		do_move = 0;\
		for(size_t j = 0; j < node_buf.busy; ++j){\
			terrain_piece* res_tpiece = node_buf.data[j].tpiece;\
			float next_z = tpiece_max_z_ceil(res_tpiece);\
			if(dx > 0){\
				if(i % bbox_w == bbox_w - 1) /*a block with the largest x coordinate*/\
					CHECK_NEXT_TPIECE_HEIGHT();\
			}\
			else if(dx < 0){\
				if(i % bbox_w == 0) /*a block with the smallest x coordinate*/\
					CHECK_NEXT_TPIECE_HEIGHT();\
			}\
			if(dy > 0){\
				if(i / bbox_w == bbox_h - 1) /*a block with the largest y coordinate*/\
					CHECK_NEXT_TPIECE_HEIGHT();\
			}\
			else if(dy < 0){\
				if(i / bbox_w == 0) /*a block with the smallest y coordinate*/\
					CHECK_NEXT_TPIECE_HEIGHT();\
			}\
			do_move = 1;\
			break;\
		}\
	}\
	if(do_move){\
		tnode* cur_node = _cur_node;\
		get_successor(dx, dy, cur_node, &node_buf);\
		for(size_t i = 0; i < node_buf.busy; ++i){\
			terrain_piece* res_tpiece = node_buf.data[i].tpiece;\
			tnode res_node = node_buf.data[i];\
			\
			float max_ceil = tpiece_max_z_ceil(res_tpiece);\
			/* collision check dx*/\
			bbox3f new_bbox = h_bbox;\
			new_bbox.min.x += _cur_node->pos.x + (dx); new_bbox.min.z += _cur_node->pos.y;\
			new_bbox.max.x += _cur_node->pos.x + (dx); new_bbox.max.z += _cur_node->pos.y;\
			new_bbox.min.y += max_ceil;\
			new_bbox.max.y += max_ceil;\
			if(bbox_check_terrain_collision(new_bbox)) continue;\
			\
			/* collision check dy*/\
			new_bbox = h_bbox;\
			new_bbox.min.x += _cur_node->pos.x; new_bbox.min.z += _cur_node->pos.y + (dy);\
			new_bbox.max.x += _cur_node->pos.x; new_bbox.max.z += _cur_node->pos.y + (dy);\
			new_bbox.min.y += max_ceil;\
			new_bbox.max.y += max_ceil;\
			if(bbox_check_terrain_collision(new_bbox)) continue;\
			\
			/* collision check dx+dy*/\
			new_bbox = h_bbox;\
			new_bbox.min.x += _cur_node->pos.x + (dx); new_bbox.min.z += _cur_node->pos.y + (dy);\
			new_bbox.max.x += _cur_node->pos.x + (dx); new_bbox.max.z += _cur_node->pos.y + (dy);\
			new_bbox.min.y += max_ceil;\
			new_bbox.max.y += max_ceil;\
			if(bbox_check_terrain_collision(new_bbox)) continue;\
\
			tnode** old_node;\
			if(!(old_node = tptr_set_find(closed, res_tpiece)) || (cur_node->cost + sqrt(dx*dx + dy*dy) == (*old_node)->cost)){\
				res_node.y = tpiece_avg_z_ceil(*res_tpiece);\
				res_node.cost = cur_node->cost + sqrt(dx*dx + dy*dy);\
				tnode_calc_heuristic(res_node, goal);\
				res_node.parent = cur_node;\
				res_node.tpiece = res_tpiece;\
				tnode* new_node_ptr = malloc(sizeof(tnode));\
				*new_node_ptr = res_node;\
				tptr_set_insert(closed, res_tpiece, new_node_ptr);\
				tnode_pqueue_push(open, new_node_ptr);\
				printf("pushed %d %d %f\n", res_node.pos.x, res_node.pos.y, max_ceil);\
			} else if(cur_node->cost + sqrt(dx*dx + dy*dy) < (*old_node)->cost){\
				(*old_node)->cost = cur_node->cost + max(dx, dy);\
				tnode_calc_heuristic(**old_node, goal);\
				(*old_node)->parent = cur_node;\
				tnode_pqueue_heapify(open);\
				printf("updated %d %d\n", res_node.pos.x, res_node.pos.y);\
			} else (*old_node)->parent ? printf("already in queue with parent %d %d\n", (*old_node)->parent->pos.x, (*old_node)->parent->pos.y) : printf("already in queue with no parent\n");\
		}\
	}\
}

void get_successor(int dx, int dy, tnode* cur_node, tnode_dynarray* node_buf)
{
	node_buf->busy = 0;

	tnode new_node;
	new_node.pos = (vec2i){cur_node->pos.x + (dx), cur_node->pos.y + (dy)};

	terrain_piece* tpiece = terrain_get_piece(new_node.pos.x, new_node.pos.y);
	terrain_piece* _tpiece_x = terrain_get_piece(new_node.pos.x, cur_node->pos.y);
	terrain_piece* _tpiece_y = terrain_get_piece(cur_node->pos.x, new_node.pos.y);

	if(tpiece){
		tpiece = tpiece->next;
		while(tpiece){
			/* ignore unpassable terrain */
			if(dx > 0){
				if(DIFF_MORE(cur_node->tpiece->z_ceil[1], tpiece->z_ceil[0]) || DIFF_MORE(cur_node->tpiece->z_ceil[2], tpiece->z_ceil[3])){ tpiece = tpiece->next; continue; }
			}
			else if(dx < 0){
				if(DIFF_MORE(cur_node->tpiece->z_ceil[0], tpiece->z_ceil[1]) || DIFF_MORE(cur_node->tpiece->z_ceil[3], tpiece->z_ceil[2])) { tpiece = tpiece->next; continue; }
			}
			if(dy > 0){
				if(DIFF_MORE(cur_node->tpiece->z_ceil[3], tpiece->z_ceil[0]) || DIFF_MORE(cur_node->tpiece->z_ceil[2], tpiece->z_ceil[1])){ tpiece = tpiece->next; continue; }
			}
			else if(dy < 0){
				if(DIFF_MORE(cur_node->tpiece->z_ceil[0], tpiece->z_ceil[3]) || DIFF_MORE(cur_node->tpiece->z_ceil[1], tpiece->z_ceil[2])) { tpiece = tpiece->next; continue; }
			}

			if(dx != 0 && dy != 0){/*check x and y neighbours of a diagonal destination*/
				if(_tpiece_x){
					terrain_piece* tpiece_x = _tpiece_x->next;
					int cont = 0;
					while(tpiece_x){
						if(dx > 0){
							if(DIFF_MORE(cur_node->tpiece->z_ceil[1], tpiece_x->z_ceil[0]) || DIFF_MORE(cur_node->tpiece->z_ceil[2], tpiece_x->z_ceil[3])){ cont = 1; break; }
						}
						else if(dx < 0){
							if(DIFF_MORE(cur_node->tpiece->z_ceil[0], tpiece_x->z_ceil[1]) || DIFF_MORE(cur_node->tpiece->z_ceil[3], tpiece_x->z_ceil[2])) { cont = 1; break; }
						}
						tpiece_x = tpiece_x->next;
					}
					if(cont) { tpiece = tpiece->next; continue; }	
				}
				if(_tpiece_y){
					terrain_piece* tpiece_y = _tpiece_y->next;
					int cont = 0;
					while(tpiece_y){
						if(dy > 0){
							if(DIFF_MORE(cur_node->tpiece->z_ceil[3], tpiece_y->z_ceil[0]) || DIFF_MORE(cur_node->tpiece->z_ceil[2], tpiece_y->z_ceil[1])){ cont = 1; break; }\
						}
						else if(dy < 0){
							if(DIFF_MORE(cur_node->tpiece->z_ceil[0], tpiece_y->z_ceil[3]) || DIFF_MORE(cur_node->tpiece->z_ceil[1], tpiece_y->z_ceil[2])) { cont = 1; break; }
						}
						tpiece_y = tpiece_y->next;
					}
					if(cont) { tpiece = tpiece->next; continue; }
				}
			}

			new_node.tpiece = tpiece;
			tnode_dynarray_push(node_buf, new_node);

			tpiece = tpiece->next;
		}
	}
}

#define PUT_TPIECE_BUF(dx, dy){\
	int _x = _cur_node->pos.x + x, _y = _cur_node->pos.y + y;\
	size_t buf_i = (x + bbox_w/2) + (y + bbox_h/2)*bbox_w;\
	if(!tpiece_buf[buf_i]){\
		size_t buf_prev_i = (x + bbox_w/2 + (dx)) + (y + bbox_h/2 + (dy))*bbox_w;\
		tnode cur_node_base = {.tpiece = tpiece_buf[buf_prev_i]};\
		if(cur_node_base.tpiece){\
			cur_node_base.pos = (vec2i){_x + (dx), _y + (dy)};\
			tnode* cur_node = &cur_node_base;\
			get_successor(-(dx), -(dy), cur_node, &node_buf);\
			for(size_t i = 0; i < node_buf.busy; ++i){\
				terrain_piece* res_tpiece = node_buf.data[i].tpiece;\
				if(!DIFF_MORE(tpiece_max_z_ceil(res_tpiece), tpiece_max_z_ceil(tpiece_buf[bbox_w/2 + bbox_h/2*bbox_w]))){\
					cur_node_base.tpiece = res_tpiece;\
					cur_node_base.pos.y += _y;\
					tpiece_buf[buf_i] = res_tpiece;\
					cur_node_base.pos.x += _x;\
					break;\
				}\
			}\
		}\
	}\
}

static void push_successors(tnode_pqueue* open, tptr_set* closed, tnode* _cur_node, vec2i goal, bbox3f h_bbox, vec3f center)
{
	printf("start %d %d\n", _cur_node->pos.x, _cur_node->pos.y);

	tnode_dynarray node_buf;
	tnode_dynarray_create(&node_buf);

	int bbox_w = ceil(h_bbox.max.x - h_bbox.min.x),
	    bbox_h = ceil(h_bbox.max.z - h_bbox.min.z);
	if(bbox_w % 2 == 0) ++bbox_w;
	if(bbox_h % 2 == 0) ++bbox_h;

	int buf_ln = bbox_w*bbox_h;
	terrain_piece** tpiece_buf = malloc(buf_ln * sizeof(terrain_piece*));
	memset(tpiece_buf, 0, buf_ln * sizeof(terrain_piece*));

	// push all blocks in buffer
	int bbox_max_side = max(bbox_w, bbox_h);
	tpiece_buf[bbox_w/2 + bbox_h/2*bbox_w] = _cur_node->tpiece;
	for(int square_side = 3; square_side <= bbox_max_side; square_side += 2){
		// TODO optimize by breaking early if square didn't yield any blocks?
		// left side
		for(int y = -square_side/2 + 1; y < square_side/2; ++y){
			int x = -square_side/2;
			PUT_TPIECE_BUF(1, 0)
			PUT_TPIECE_BUF(1, -1)
			PUT_TPIECE_BUF(1, 1)
		}
		// right side
		for(int y = -square_side/2 + 1; y < square_side/2; ++y){
			int x = square_side/2;
			PUT_TPIECE_BUF(-1, 0)
			PUT_TPIECE_BUF(-1, -1)
			PUT_TPIECE_BUF(-1, 1)
		}
		// top side
		for(int x = -square_side/2; x <= square_side/2; ++x){
			int y = square_side/2;
			PUT_TPIECE_BUF(0, -1)
			PUT_TPIECE_BUF(-1, -1)
			PUT_TPIECE_BUF(1, -1)
		}
		// bottom side
		for(int x = -square_side/2; x <= square_side/2; ++x){
			int y = -square_side/2;
			PUT_TPIECE_BUF(0, 1)
			PUT_TPIECE_BUF(-1, 1)
		PUT_TPIECE_BUF(1, 1)
		}
	}

	PUSH_SUCCESSOR(1, 0)
	PUSH_SUCCESSOR(0, 1)
	PUSH_SUCCESSOR(-1, 0)
	PUSH_SUCCESSOR(0, -1)
	PUSH_SUCCESSOR(1, 1)
	PUSH_SUCCESSOR(-1, 1)
	PUSH_SUCCESSOR(1, -1)
	PUSH_SUCCESSOR(-1, -1)
	printf("end\n");
}
path path_find(const hexahedron* h, vec3f target)
{
	// figure out the starting point
	vec3f center = hexahedron_get_center(h);
	vec2i target_xz = {target.x, target.z};
	bbox3f h_bbox = hexahedron_get_bbox(h);
	h_bbox.min = vec3_sub(h_bbox.min, center);
	h_bbox.max = vec3_sub(h_bbox.max, center);
	float h_bbox_y = h_bbox.min.y;
	h_bbox.min.y -= h_bbox_y; h_bbox.max.y -= h_bbox_y;
	h_bbox.min.x += 0.5; h_bbox.min.z += 0.5;
	h_bbox.max.x += 0.5; h_bbox.max.z += 0.5;

	tnode* cur_node = malloc(sizeof(tnode));
	cur_node->cost = 0; cur_node->parent = NULL;
	cur_node->pos = (vec2i){center.x, center.z};
	cur_node->tpiece = terrain_get_piece(cur_node->pos.x, cur_node->pos.y);
	printf("PATH FIND START %d %d\n", cur_node->pos.x, cur_node->pos.y);
	if(!cur_node->tpiece) return (path){0, NULL};
	cur_node->tpiece = get_nearest_tpiece(center.y, cur_node->tpiece->next);
	if(!cur_node->tpiece) return (path){0, NULL};
	cur_node->y = tpiece_avg_z_ceil(*cur_node->tpiece);
	tnode_calc_heuristic(*cur_node, target_xz);

	tptr_set closed;
	tptr_set_create(&closed, 16, tptr_hash);
	tnode_pqueue open;
	tnode_pqueue_create(&open, tnode_cmp);
	tptr_set_insert(&closed, cur_node->tpiece, cur_node);
	tnode_pqueue_push(&open, cur_node);

	while(!tnode_pqueue_is_empty(&open)){
		cur_node = tnode_pqueue_pop(&open);
		if(vec2_eq(cur_node->pos, target_xz))
			break;
		push_successors(&open, &closed, cur_node, target_xz, h_bbox, center);
	}

	tnode* n = cur_node;
	if(!vec2_eq(n->pos, target_xz)){ // haven't reached the goal
		for(size_t i = 0; i < closed.size; ++i)
			if(tptr_set_is_allocated(&closed, i))
				free(closed.data[i]);
		tnode_pqueue_destroy(&open);
		tptr_set_destroy(&closed);
		return (path){0, NULL};
	}

	path out_p = {.ln = 0};
	while(n){
		++out_p.ln;
		n = n->parent;
	}
	n = cur_node;
	out_p.points = malloc(sizeof(vec3f) * out_p.ln);
	for(size_t i = 0; n; ++i){
		printf("%d %d\n", n->pos.x, n->pos.y);
		out_p.points[out_p.ln - i - 1] = (vec2f){n->pos.x, n->pos.y};
		n = n->parent;
	}
	// free resources
	for(size_t i = 0; i < closed.size; ++i)
		if(tptr_set_is_allocated(&closed, i))
			free(closed.data[i]);
	tnode_pqueue_destroy(&open);
	tptr_set_destroy(&closed);

	return out_p;
}
