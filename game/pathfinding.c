#include "game/pathfinding.h"
#include "htable_oa.h"
#include "pqueue.h"

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

#define PUSH_SUCCESSOR(dx, dy){\
	tnode new_node;\
	new_node.pos = (vec2i){cur_node->pos.x + (dx), cur_node->pos.y + (dy)};\
	terrain_piece* tpiece = terrain_get_piece(new_node.pos.x, new_node.pos.y);\
	if(tpiece){\
		tpiece = tpiece->next;\
		while(tpiece){\
			/* ignore unpassable terrain */\
			if(dx > 0){\
				if(cur_node->tpiece->z_ceil[1] < tpiece->z_ceil[0] || cur_node->tpiece->z_ceil[2] < tpiece->z_ceil[3]){ tpiece = tpiece->next; continue; }\
			}\
			else if(dx < 0){\
				if(cur_node->tpiece->z_ceil[0] < tpiece->z_ceil[1] || cur_node->tpiece->z_ceil[3] < tpiece->z_ceil[2]) { tpiece = tpiece->next; continue; }\
			}\
			if(dy > 0){\
				if(cur_node->tpiece->z_ceil[3] < tpiece->z_ceil[0] || cur_node->tpiece->z_ceil[2] < tpiece->z_ceil[1]){ tpiece = tpiece->next; continue; }\
			}\
			else if(dy < 0){\
				if(cur_node->tpiece->z_ceil[0] < tpiece->z_ceil[3] || cur_node->tpiece->z_ceil[1] < tpiece->z_ceil[2]) { tpiece = tpiece->next; continue; }\
			}\
			/* ignore the cell if it's unreachable due to (possible) collisions */\
			bbox3f new_bbox = h_bbox;\
			new_bbox.min.x += new_node.pos.x; new_bbox.min.z += cur_node->pos.y;\
			new_bbox.max.x += new_node.pos.x; new_bbox.max.z += cur_node->pos.y;\
			new_bbox.min.y += tpiece_avg_z_ceil(*tpiece);\
			new_bbox.max.y += tpiece_avg_z_ceil(*tpiece);\
			if(bbox_check_terrain_collision(new_bbox)){\
				tpiece = tpiece->next; continue;\
			}\
			new_bbox = h_bbox;\
			new_bbox.min.x += cur_node->pos.x; new_bbox.min.z += new_node.pos.y;\
			new_bbox.max.x += cur_node->pos.x; new_bbox.max.z += new_node.pos.y;\
			new_bbox.min.y += tpiece_avg_z_ceil(*tpiece);\
			new_bbox.max.y += tpiece_avg_z_ceil(*tpiece);\
			if(bbox_check_terrain_collision(new_bbox)){\
				tpiece = tpiece->next; continue;\
			}\
			\
			tnode** old_node;\
			if(!(old_node = tptr_set_find(closed, tpiece)) ){\
				new_node.y = tpiece_avg_z_ceil(*tpiece);\
				new_node.cost = cur_node->cost + max(dx, dy);\
				tnode_calc_heuristic(new_node, goal);\
				new_node.parent = cur_node;\
				new_node.tpiece = tpiece;\
				tnode* new_node_ptr = malloc(sizeof(tnode));\
				*new_node_ptr = new_node;\
				tptr_set_insert(closed, tpiece, new_node_ptr);\
				tnode_pqueue_push(open, new_node_ptr);\
				printf("pushed %d %d\n", new_node.pos.x, new_node.pos.y);\
			} else if(cur_node->cost + max(dx, dy) < (*old_node)->cost){\
				(*old_node)->cost = cur_node->cost + max(dx, dy);\
				tnode_calc_heuristic(**old_node, goal);\
				(*old_node)->parent = cur_node;\
				tnode_pqueue_heapify(open);\
			}\
			tpiece = tpiece->next;\
		}\
	}\
}
static void push_successors(tnode_pqueue* open, tptr_set* closed, tnode* cur_node, vec2i goal, bbox3f h_bbox)
{
	printf("start\n");
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
		push_successors(&open, &closed, cur_node, target_xz, h_bbox);
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
