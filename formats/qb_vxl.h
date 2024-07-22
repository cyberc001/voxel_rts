#ifndef QB_VXL_H
#define QB_VXL_H

#include <stdio.h>
#include "math/vec.h"
#include "render/base.h"
#include "dyn_array.h"

typedef struct {
	render_obj model;
	vec3f origin;
	vec3f size;
} voxel_model;
DEF_DYN_ARRAY(voxel_array, voxel_model)

voxel_array read_qb_vxl(FILE* fd);

#endif
