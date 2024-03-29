#include "controls/selection.h"

#include "math/vec.h"
#include "controls.h"

static int is_selecting = 0;
int controls_selection_queried = 0;
vec2f controls_selection_start, controls_selection_end;

static void selection_kb_down(key_code key, int mods)
{
	if(check_key_bind(key, "select")) {
		is_selecting = 1;
		controls_selection_start = get_mouse_coords();
	}
}
static void selection_kb_up(key_code key, int mods)
{
	if(check_key_bind(key, "select")) {
		is_selecting = 0;
		controls_selection_end = get_mouse_coords();
		controls_selection_queried = 1;
	}
}

void controls_init_selection()
{
	controls_add_key_down(selection_kb_down);
	controls_add_key_up(selection_kb_up);
}
