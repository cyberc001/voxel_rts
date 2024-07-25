#include "render/primitive.h"

render_obj generate_hexahedron(const hexahedron* h)
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
render_obj generate_box(vec3f size)
{
	float* verts = malloc(sizeof(float)*72);
	int face_end_bits[] = {0x3, 0x5, 0x6, 0x7, 0x7, 0x7};
	int face_start_bits[] = {0x0, 0x0, 0x0, 0x1, 0x2, 0x4};
	for(size_t i = 0; i < 6; ++i){
		vec3f start = {
			size.x/2 * (face_start_bits[i] & 1 ? 1 : -1),
			size.y/2 * (face_start_bits[i] & 2 ? 1 : -1),
			size.z/2 * (face_start_bits[i] & 4 ? 1 : -1)
		};
		vec3f end = {
			size.x/2 * (face_end_bits[i] & 1 ? 1 : -1),
			size.y/2 * (face_end_bits[i] & 2 ? 1 : -1),
			size.z/2 * (face_end_bits[i] & 4 ? 1 : -1)
		};

		verts[i*4*3   ] = start.x;
		verts[i*4*3+1 ] = start.y;
		verts[i*4*3+2 ] = start.z;

		int diff = face_start_bits[i] ^ face_end_bits[i];
		if(diff & 1){
			verts[i*4*3+3 ] = end.x;
			verts[i*4*3+4 ] = start.y;
			verts[i*4*3+5 ] = start.z;
		} else { // diff & 2
			verts[i*4*3+3 ] = start.x;
			verts[i*4*3+4 ] = end.y;
			verts[i*4*3+5 ] = start.z;
		}

		verts[i*4*3+6] = end.x;
		verts[i*4*3+7] = end.y;
		verts[i*4*3+8] = end.z;

		if(diff & 1){
			if(diff & 2){
				verts[i*4*3+9 ] = start.x;
				verts[i*4*3+10] = end.y;
				verts[i*4*3+11] = start.z;
			} else { // diff & 4
				verts[i*4*3+9 ] = start.x;
				verts[i*4*3+10] = start.y;
				verts[i*4*3+11] = end.z;
			}
		} else { // (diff & 2) && (diff & 4)
			verts[i*4*3+9 ] = start.x;
			verts[i*4*3+10] = start.y;
			verts[i*4*3+11] = end.z;
		}
	}

	float* color = malloc(sizeof(float)*72);
	for(size_t i = 0; i < 72; ++i) color[i] = 1;
	return render_obj_generate(GL_POLYGON, RENDER_OBJ_FLAG_WIREFRAME | RENDER_OBJ_FLAG_ALLOCED, verts, sizeof(float)*72,
					RENDER_OBJ_COLOR, color, sizeof(float)*72,
					RENDER_OBJ_ATTRIBUTES_END);
}
