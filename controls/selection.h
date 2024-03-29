#ifndef CONTROLS_SELECTION_H
#define CONTROLS_SELECTION_H

#include "math/vec.h"

extern int controls_selection_queried;
extern vec2f controls_selection_start, controls_selection_end;

void controls_init_selection();

#endif
