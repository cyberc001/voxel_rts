#ifndef RENDER_PRIMITIVE_H
#define RENDER_PRIMITIVE_H

#include "render/base.h"
#include "math/hexahedron.h"
#include "math/box.h"

render_obj generate_hexahedron(const hexahedron* h);
render_obj generate_box(vec3f size);

#endif
