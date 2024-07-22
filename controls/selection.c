#include "controls/selection.h"

#include "controls.h"
#include "game/terrain.h"

int controls_is_selecting = 0;
vec2f controls_selection_start;
vec2f controls_selection_min, controls_selection_max;

vec3f controls_order_world_coords;

int controls_selection_queried = 0;
int controls_order_queried = 0;

static void selection_kb_down(key_code key, int mods)
{
	if(check_key_bind(key, "select")){
		controls_is_selecting = 1;
		controls_selection_start = get_mouse_coords();
		TRANSLATE_ORTHO_COORDS(controls_selection_start);
	}
}
static void selection_kb_up(key_code key, int mods)
{
	int sel_query = CONTROLS_SELECTION_NOT_QUERIED;
	int order_query = 0;
	if(check_key_bind(key, "select")){
		controls_is_selecting = 0;
		vec2f controls_selection_end = get_mouse_coords();
		TRANSLATE_ORTHO_COORDS(controls_selection_end);

		controls_selection_min = (vec2f){min(controls_selection_start.x, controls_selection_end.x),
				min(controls_selection_start.y, controls_selection_end.y)};
		controls_selection_max = (vec2f){max(controls_selection_start.x, controls_selection_end.x),
				max(controls_selection_start.y, controls_selection_end.y)};
		vec2f delta = vec2_sub(controls_selection_max, controls_selection_min);
		if(delta.x > UI_SELECTION_MIN && delta.y > UI_SELECTION_MIN)
			sel_query = CONTROLS_SELECTION_QUERIED_AREA;
		else
			sel_query = CONTROLS_SELECTION_QUERIED_SINGLE_POINT;
	}
	else if(check_key_bind(key, "order")){
		controls_selection_min = (vec2f){NAN, NAN};
		sel_query = CONTROLS_SELECTION_QUERIED_SINGLE_POINT;
	}

	if(sel_query == CONTROLS_SELECTION_QUERIED_SINGLE_POINT && check_key_bind(key, "order")){
		vec2f mouse = get_mouse_coords();
		TRANSLATE_ORTHO_COORDS(mouse);

		// Project a line from camera until distance limit is exhaused or terrain piece is found
		vec3f proj = vec2f_project3(mouse);

		proj = vec3_norm(proj);
		proj = vec3_smul(proj, 100);

		vec3f start = get_camera_center(),
		      end = vec3_add(render_cam_pos, proj);
		terrain_piece* tpiece = terrain_find_first_piece_in_line((line3f){start, end}, &controls_order_world_coords);
		if(tpiece)
			order_query = 1;
	}

	controls_order_queried = order_query;
	controls_selection_queried = sel_query;
}

void controls_init_selection()
{
	controls_add_key_down(selection_kb_down);
	controls_add_key_up(selection_kb_up);
}
