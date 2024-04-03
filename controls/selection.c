#include "controls/selection.h"

#include "math/vec.h"
#include "controls.h"

int controls_is_selecting = 0;
vec2f controls_selection_start;
int controls_selection_queried = 0;
vec2f controls_selection_min, controls_selection_max;

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
	if(check_key_bind(key, "select")){
		controls_is_selecting = 0;
		vec2f controls_selection_end = get_mouse_coords();
		TRANSLATE_ORTHO_COORDS(controls_selection_end);

		controls_selection_min = (vec2f){min(controls_selection_start.x, controls_selection_end.x),
				min(controls_selection_start.y, controls_selection_end.y)};
		controls_selection_max = (vec2f){max(controls_selection_start.x, controls_selection_end.x),
				max(controls_selection_start.y, controls_selection_end.y)};

		controls_selection_queried = 1;
	}
}

void controls_init_selection()
{
	controls_add_key_down(selection_kb_down);
	controls_add_key_up(selection_kb_up);
}
