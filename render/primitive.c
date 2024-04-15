#include "render/primitive.h"

#define face_xyz(i) h.f[i].p[0].x, h.f[i].p[0].y, h.f[i].p[0].z, h.f[i].p[1].x, h.f[i].p[1].y, h.f[i].p[1].z, h.f[i].p[2].x, h.f[i].p[2].y, h.f[i].p[2].z, h.f[i].p[3].x, h.f[i].p[3].y, h.f[i].p[3].z

render_obj render_hexahedron(hexahedron h)
{
	float verts[] = {face_xyz(0), face_xyz(1), face_xyz(2), face_xyz(3), face_xyz(4), face_xyz(5)};
	float color[72]; for(size_t i = 0; i < 72; ++i) color[i] = 1;
	size_t i = 5;
	color[i*3] = color[i*3 + 1] = 0; color[i*3 + 2] = 1;
	i = 1;
	color[i*3] = color[i*3 + 1] = 0; color[i*3 + 2] = 1;
	i = 2;
	color[i*3] = color[i*3 + 1] = 0; color[i*3 + 2] = 1;
	i = 6;
	color[i*3] = color[i*3 + 1] = 0; color[i*3 + 2] = 1;
	return render_obj_create(GL_POLYGON, RENDER_OBJ_FLAG_WIREFRAME, verts, sizeof(verts),
					RENDER_OBJ_COLOR, color, sizeof(color),
					RENDER_OBJ_ATTRIBUTES_END);
}
