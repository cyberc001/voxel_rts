#ifndef CONTROLS_SELECTION_H
#define CONTROLS_SELECTION_H

#include "math/vec.h"

#define UI_SELECTION_MIN	0.01

extern int controls_is_selecting;
extern vec2f controls_selection_start;
extern vec2f controls_selection_min, controls_selection_max;

extern vec3f controls_order_world_coords;

extern int controls_selection_queried;
extern int controls_order_queried;

void controls_init_selection();

#endif
