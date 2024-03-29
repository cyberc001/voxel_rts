#ifndef CONTROLS_SELECTION_H
#define CONTROLS_SELECTION_H

#include "math/vec.h"

extern int controls_selection_queried;
extern vec2f controls_selection_min, controls_selection_max;

void controls_init_selection();

#endif
