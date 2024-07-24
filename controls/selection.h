#ifndef CONTROLS_SELECTION_H
#define CONTROLS_SELECTION_H

#include "math/vec.h"

#define UI_SELECTION_MIN	0.01

#define CONTROLS_SELQUERY_NONE			0
#define CONTROLS_SELQUERY_AREA			1
#define CONTROLS_SELQUERY_POINT			2
#define CONTROLS_SELQUERY_GROUND		3

extern int controls_is_selecting;
extern vec2f controls_selection_start;
extern vec2f controls_selection_min, controls_selection_max;

extern vec3f controls_order_world_coords;

extern int controls_selection_queried;

void controls_init_selection();

#endif
