#include "render/primitive.h"

render_obj render_hexahedron(const hexahedron* h)
{
	float* verts = malloc(sizeof(float)*72);
	for(size_t i = 0; i < 6; ++i)
		for(size_t j = 0; j < 4; ++j){
			verts[i*4*3 + j*3] = h->f[i].p[j].x;
			verts[i*4*3 + j*3 + 1] = h->f[i].p[j].y;
			verts[i*4*3 + j*3 + 2] = h->f[i].p[j].z;
		}
	float* color = malloc(sizeof(float)*72);
	for(size_t i = 0; i < 72; ++i) color[i] = 1;

	return render_obj_generate(GL_POLYGON, RENDER_OBJ_FLAG_WIREFRAME | RENDER_OBJ_FLAG_ALLOCED, verts, sizeof(float)*72,
					RENDER_OBJ_COLOR, color, sizeof(float)*72,
					RENDER_OBJ_ATTRIBUTES_END);
}
