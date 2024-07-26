#include "game/pathfinding.h"
#include "math/quat.h"
#include "htable_oa.h"
#include "pqueue.h"
#include "game/logic.h"

static float heuristic(vec3f v1, vec3f v2)
{
	vec3f dist = vec3_sub(v1, v2);
	return dist.x*dist.x + dist.y*dist.y + dist.z*dist.z; // avoid square root - doesn't matter when comparing distances
}
#define tnode_calc_heuristic(t, goal, goal_y) ( (t).heuristic = heuristic((vec3f){(t).x, (t).y, (t).z}, (vec3f){(goal).x, (goal_y), (goal).y}) )

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

#define DIFF_MORE(a, b) (fabs((a) - (b)) > 0.1)

terrain_piece* get_successor(int dx, int dz, tnode* cur_node)
{
	int x = cur_node->x, z = cur_node->z;
	terrain_piece* max_y_tpiece = NULL;
	float max_y_ceil = -INFINITY;

	terrain_piece* tpiece = terrain_get_piece(x + (dx), z + (dz));
	terrain_piece* _tpiece_x = terrain_get_piece(x + (dx), z);
	terrain_piece* _tpiece_z = terrain_get_piece(x, z + (dz));

	while(tpiece){
		float y_ceil = tpiece_max_y_ceil(tpiece);
		if(y_ceil < max_y_ceil){
			tpiece = tpiece->next; continue;
		}

		/* ignore unpassable terrain */
		if(dx > 0){
			if(DIFF_MORE(cur_node->tpiece->y_ceil[1], tpiece->y_ceil[0]) || DIFF_MORE(cur_node->tpiece->y_ceil[2], tpiece->y_ceil[3])){ tpiece = tpiece->next; continue; }
		}
		else if(dx < 0){
			if(DIFF_MORE(cur_node->tpiece->y_ceil[0], tpiece->y_ceil[1]) || DIFF_MORE(cur_node->tpiece->y_ceil[3], tpiece->y_ceil[2])) { tpiece = tpiece->next; continue; }
		}
		if(dz > 0){
			printf("%f %f %f %f\n", cur_node->tpiece->y_ceil[3], tpiece->y_ceil[0], cur_node->tpiece->y_ceil[2], tpiece->y_ceil[1]);
			if(DIFF_MORE(cur_node->tpiece->y_ceil[3], tpiece->y_ceil[0]) || DIFF_MORE(cur_node->tpiece->y_ceil[2], tpiece->y_ceil[1])){ tpiece = tpiece->next; continue; }
		}
		else if(dz < 0){
			if(DIFF_MORE(cur_node->tpiece->y_ceil[0], tpiece->y_ceil[3]) || DIFF_MORE(cur_node->tpiece->y_ceil[1], tpiece->y_ceil[2])) { tpiece = tpiece->next; continue; }
		}
		if(dx != 0 && dz != 0){/*check x and y neighbours of a diagonal destination*/
			terrain_piece* tpiece_x = _tpiece_x;
			int cont = 0;
			while(tpiece_x){
				if(dx > 0){
					if(DIFF_MORE(cur_node->tpiece->y_ceil[1], tpiece_x->y_ceil[0]) || DIFF_MORE(cur_node->tpiece->y_ceil[2], tpiece_x->y_ceil[3])){ cont = 1; break; }
				}
				else if(dx < 0){
					if(DIFF_MORE(cur_node->tpiece->y_ceil[0], tpiece_x->y_ceil[1]) || DIFF_MORE(cur_node->tpiece->y_ceil[3], tpiece_x->y_ceil[2])) { cont = 1; break; }
				}
				tpiece_x = tpiece_x->next;
			}
			if(cont) { tpiece = tpiece->next; continue; }	
		
			terrain_piece* tpiece_z = _tpiece_z;
			cont = 0;
			while(tpiece_z){
				if(dz > 0){
					if(DIFF_MORE(cur_node->tpiece->y_ceil[3], tpiece_z->y_ceil[0]) || DIFF_MORE(cur_node->tpiece->y_ceil[2], tpiece_z->y_ceil[1])){ cont = 1; break; }\
				}
				else if(dz < 0){
					if(DIFF_MORE(cur_node->tpiece->y_ceil[0], tpiece_z->y_ceil[3]) || DIFF_MORE(cur_node->tpiece->y_ceil[1], tpiece_z->y_ceil[2])) { cont = 1; break; }
				}
				tpiece_z = tpiece_z->next;
			}
			if(cont) { tpiece = tpiece->next; continue; }
		}

		max_y_ceil = y_ceil;
		max_y_tpiece = tpiece;
		tpiece = tpiece->next;
	}
	return max_y_tpiece;
}

static void push_successor(tnode_pqueue* open, tptr_set* closed,
				body* b,
				vec2i goal, float goal_y,
				tnode* cur_node, int dx, int dz)
{
	printf("trying to push successor %d %d (%g)\n", cur_node->x + dx, cur_node->z + dz, cur_node->y);

	// check for collision
	terrain_piece* suc = get_successor(dx, dz, cur_node);
	if(!suc)
		return;
	float new_y = tpiece_avg_y_ceil(*suc);

	// get upper surface normal
	vec3f p1 = {cur_node->x + dx, suc->y_ceil[0], cur_node->z + dz};
	vec3f p2 = {cur_node->x + dx + 1, suc->y_ceil[1], cur_node->z + dz};
	vec3f p3 = {cur_node->x + dx + 1, suc->y_ceil[2], cur_node->z + dz + 1};
	vec3f e1 = vec3_sub(p1, p2), e2 = vec3_sub(p2, p3);
	vec3f norm = vec3_norm(vec3_cross(e1, e2));
	printf("norm: "); vec3f_print(norm);

	mat4f trmat = mat4f_identity();
	vec3f up = vec3_smul(norm, -1);
	vec3f forward = (vec3f){dx, 0, dz};
	forward = vec3_norm(forward);
	vec3f n = (vec3f){0, 1, 0};
	float forward_ang = rad_to_ang(acos(vec3_dot(n, up)));
	vec3f forward_axis = vec3_norm(vec3_cross(n, up));
	mat4f forward_trmat = mat4f_identity();
	if(!isnan(forward_axis.x))
	mat4f_rotate(&forward_trmat, forward_ang, forward_axis);
	forward = vec3_norm(mat4f_mul_vec3f(&forward_trmat, forward));
	vec3f right = vec3_norm(vec3_cross(forward, up));

	trmat.e[0][0] = forward.x;
	trmat.e[0][1] = forward.y;
	trmat.e[0][2] = forward.z;
	trmat.e[1][0] = up.x;
	trmat.e[1][1] = up.y;
	trmat.e[1][2] = up.z;
	trmat.e[2][0] = right.x;
	trmat.e[2][1] = right.y;
	trmat.e[2][2] = right.z;

	
	//if(dx){
		body* newb = malloc(b->size);
		memcpy(newb, b, b->size);
		newb->transform.pos = (vec3f){0, 0, 0};
		newb->transform.rot = quat_from_rot_mat(&trmat);
		newb->transform.dirty = 1;

		bbox3f base_bbox = body_get_bbox(newb);
		float base_y_off = base_bbox.min.y;

		newb->transform.pos = (vec3f){cur_node->x + dx + 0.5, new_y - base_y_off, cur_node->z + dz + 0.5};
		newb->transform.dirty = 1;

		printf("new pos: "); vec3f_print(newb->transform.pos);
		printf("new rot: "); vec4f_print(newb->transform.rot);

		vec3f resol;
		if(sat_check_terrain_collision(newb, &resol, (vec3f){1, 0, 0}, NULL)
				&& vec3_ln(resol) >= 0.1){
			printf("resol: "); vec3f_print(resol);
			return;
		}
		printf("resol: "); vec3f_print(resol);
		bbox3f bbox = body_get_bbox(newb);
		if(check_bbox_octree_collision(global_lua_state, bbox))
			return;
	//}
	
	// add to queue
	tnode** old_node;
	float dcost = sqrt(dx*dx + dz*dz);
	if(!(old_node = tptr_set_find(closed, suc)) || (cur_node->cost + dcost == (*old_node)->cost)){ // tpiece wasn't encountered yet or has the same cost
		tnode rn = {
			.x = cur_node->x + dx, .z = cur_node->z + dz,
			.y = tpiece_max_y_ceil(suc),
			.cost = cur_node->cost + dcost,
			.parent = cur_node,
			.tpiece = suc
		};
		tnode_calc_heuristic(rn, goal, goal_y);

		tnode* rn_ptr = malloc(sizeof(tnode));
		*rn_ptr = rn;
		tptr_set_insert(closed, suc, rn_ptr);
		tnode_pqueue_push(open, rn_ptr);
		printf("pushed %d %f %d (%f %f)\n", rn.x, rn.y, rn.z, rn.cost, rn.heuristic);
	} else if(cur_node->cost + dcost < (*old_node)->cost){ // tpiece is already in closed set and node has a different cost; update cost and the parent
		(*old_node)->cost = cur_node->cost + dcost;
		tnode_calc_heuristic(**old_node, goal, goal_y);
		(*old_node)->parent = cur_node;
		tnode_pqueue_heapify(open);
	printf("pushed %d %f %d (%f %f)\n", (*old_node)->x, (*old_node)->y, (*old_node)->z, (*old_node)->cost, (*old_node)->heuristic);
	}
}

#define PUSH_SUCCESSOR(dx, dz)\
	push_successor(open, closed, b, (vec2i){goal_x, goal_z}, goal_y, cur_node, (dx), (dz));
static void process_node(tnode_pqueue* open, tptr_set* closed, body* b, tnode* cur_node, int goal_x, float goal_y, int goal_z)
{
	printf("start %d %d\n", cur_node->x, cur_node->z);
	
	PUSH_SUCCESSOR(1, 0)
	PUSH_SUCCESSOR(0, 1)
	PUSH_SUCCESSOR(-1, 0)
	PUSH_SUCCESSOR(0, -1)
	PUSH_SUCCESSOR(1, 1)
	PUSH_SUCCESSOR(-1, 1)
	PUSH_SUCCESSOR(1, -1)
	PUSH_SUCCESSOR(-1, -1)
}

path path_find(body* b, vec3f target, int pathing_type, ...)
{
	/* Determine current position */
	tnode* cur_node = malloc(sizeof(tnode));
	cur_node->cost = 0; cur_node->parent = NULL;
	cur_node->x = floor(b->transform.pos.x); cur_node->z = floor(b->transform.pos.z);
	cur_node->tpiece = terrain_get_piece(cur_node->x, cur_node->z);
	if(!cur_node->tpiece){
		free(cur_node);
		return (path){0, NULL, NULL};
	}
	cur_node->tpiece = terrain_get_nearest_piece_maxy(b->transform.pos.y, cur_node->tpiece);
	cur_node->y = tpiece_max_y_ceil(cur_node->tpiece);
	printf("PATH FIND START %d %g %d\n", cur_node->x, cur_node->y, cur_node->z);

	/* Determine target position */
	vec2i target_xz = {target.x, target.z};
	tnode_calc_heuristic(*cur_node, target_xz, target.y);

	/* Initialize closed set and open priority queue */
	tptr_set closed;
	tptr_set_create(&closed, 16, tptr_hash);
	tnode_pqueue open;
	tnode_pqueue_create(&open, tnode_cmp);
	tptr_set_insert(&closed, cur_node->tpiece, cur_node);
	tnode_pqueue_push(&open, cur_node);

	/* Initialize conditional pathing */
	float goal_distance;
	vec2f pitch_range;
	va_list args;
	va_start(args, pathing_type);
	switch(pathing_type){
		case PATHING_TYPE_DISTANCE:
			goal_distance = va_arg(args, double);
			pitch_range = va_arg(args, vec2f);
			break;
	}
	va_end(args);

	int reached_goal = 0;
	while(!tnode_pqueue_is_empty(&open)){
		cur_node = tnode_pqueue_pop(&open);
		switch(pathing_type){
			case PATHING_TYPE_EXACT:
				if(cur_node->x == target_xz.x && fabs(cur_node->y - target.y) <= 0.6 && cur_node->z == target_xz.y){
					reached_goal = 1;
					goto astar_end;
				}
				break;
			case PATHING_TYPE_DISTANCE: {
				vec3f cur_pos = (vec3f){cur_node->x + 0.5, cur_node->y, cur_node->z + 0.5};
				vec3f diff = vec3_sub(target, cur_pos);

				printf("diff "); vec3f_print(diff);
				vec3f rot = rot_from_quat(quat_from_rot_between(diff, (vec3f){diff.x, 0, diff.z}));
				printf("rot"); vec3f_print(rot);
				float pitch = rot.y;
				if(isnan(pitch)) pitch = 0;
				if(pitch > 90) pitch = 180 - pitch;
				if(pitch < -90) pitch = -(180 + pitch);

				printf("PITCH %f < %f < %f\n", pitch_range.x, pitch, pitch_range.y);
				printf("DIST %f %f\n", vec3_ln(diff), goal_distance);

				if(vec3_ln(diff) <= goal_distance
				&& in_range(pitch, pitch_range.x, pitch_range.y)){
					reached_goal = 1;
					goto astar_end;
				}
				break;
			}
		}
		process_node(&open, &closed, b, cur_node, target_xz.x, target.y, target_xz.y);
	}
astar_end:

	tnode* n = cur_node;
	if(!reached_goal){
		for(size_t i = 0; i < closed.size; ++i)
			if(tptr_set_is_allocated(&closed, i))
				free(closed.data[i]);
		tnode_pqueue_destroy(&open);
		tptr_set_destroy(&closed);
		return (path){0, NULL, NULL};
	}

	path out_p = {.ln = 0};
	while(n){
		++out_p.ln;
		n = n->parent;
	}
	n = cur_node;
	out_p.points = malloc(sizeof(vec3f) * out_p.ln);
	out_p.tpieces = malloc(sizeof(terrain_piece*) * out_p.ln);
	for(size_t i = 0; n; ++i){
		printf("%d %f %d\n", n->x, n->y, n->z);
		out_p.points[out_p.ln - i - 1] = (vec2f){n->x, n->z};
		out_p.tpieces[out_p.ln - i - 1] = n->tpiece;
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
