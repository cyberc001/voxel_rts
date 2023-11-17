#include "formats/qb_vxl.h"
#include <stdint.h>

#define CLR_FORMAT_RGBA		0
#define CLR_FORMAT_BGRA		1

#define AXIS_ORIENT_LEFT	0
#define AXIS_ORIENT_RIGHT	1

#define COMPRESSION_NONE	0
#define COMPRESSION_RLE		1

typedef struct {
	uint32_t version;
	uint32_t clr_format;
	uint32_t axis_orient;
	uint32_t compression;
	uint32_t visibility_mask;
	uint32_t matrix_cnt;
} qb_hdr;

#define VOXEL_ALLOC_FRACTION	36
#define VOXEL_SCALE 		0.05

#define FLAG_CODE		2
#define FLAG_NEXT_SLICE		6

#define READ_VAR(type, name)\
	type name;\
	fread(&name, sizeof(name), 1, fd);


#define MODEL_SET_DATA(x, y, z, r, g, b){\
	model_vert[model_idx] = ((x) - m_info.size_x / 2) * VOXEL_SCALE;\
	model_vert[model_idx + 1] = ((y) - m_info.size_y / 2) * VOXEL_SCALE;\
	model_vert[model_idx + 2] = ((z) - m_info.size_z / 2) * VOXEL_SCALE;\
	model_clr[model_idx] = r;\
	model_clr[model_idx + 1] = g;\
	model_clr[model_idx + 2] = b;\
	model_idx += 3;\
}

#define GET_VOXEL_RGB(data)\
	GLfloat r = ((data) & 0xFF) / 255., g = (((data) & 0xFF00) >> 8) / 255., b = (((data) & 0xFF0000) >> 16) / 255.\

#define SET_VOXEL_DATA(data, x, y, z){\
	if(model_idx + 12*6 >= model_ln){\
		model_ln = model_ln * 2 > model_idx + 12*6 ? model_ln * 2 : model_ln + 12 * 6;\
		model_vert = realloc(model_vert, sizeof(GLfloat) * model_ln);\
		model_clr = realloc(model_clr, sizeof(GLfloat) * model_ln);\
	}\
	if(hdr.visibility_mask && (data) & 0xFF000000){\
		GET_VOXEL_RGB(data);\
		uint32_t flags = (data) >> 24;\
		if(flags & 32){\
			MODEL_SET_DATA(x, y, z, r, g, b);\
			MODEL_SET_DATA(x + 1, y, z, r, g, b);\
			MODEL_SET_DATA(x + 1, y + 1, z, r, g, b);\
			MODEL_SET_DATA(x, y + 1, z, r, g, b);\
		}\
		if(flags & 64){\
			MODEL_SET_DATA(x, y, z + 1, r, g, b);\
			MODEL_SET_DATA(x + 1, y, z + 1, r, g, b);\
			MODEL_SET_DATA(x + 1, y + 1, z + 1, r, g, b);\
			MODEL_SET_DATA(x, y + 1, z + 1, r, g, b);\
		}\
		if(flags & 16){\
			MODEL_SET_DATA(x, y, z, r, g, b);\
			MODEL_SET_DATA(x + 1, y, z, r, g, b);\
			MODEL_SET_DATA(x + 1, y, z + 1, r, g, b);\
			MODEL_SET_DATA(x, y, z + 1, r, g, b);\
		}\
		if(flags & 8){\
			MODEL_SET_DATA(x, y + 1, z, r, g, b);\
			MODEL_SET_DATA(x + 1, y + 1, z, r, g, b);\
			MODEL_SET_DATA(x + 1, y + 1, z + 1, r, g, b);\
			MODEL_SET_DATA(x, y + 1, z + 1, r, g, b);\
		}\
		if(flags & 4){\
			MODEL_SET_DATA(x, y, z, r, g, b);\
			MODEL_SET_DATA(x, y + 1, z, r, g, b);\
			MODEL_SET_DATA(x, y + 1, z + 1, r, g, b);\
			MODEL_SET_DATA(x, y, z + 1, r, g, b);\
		}\
		if(flags & 2){\
			MODEL_SET_DATA(x + 1, y, z, r, g, b);\
			MODEL_SET_DATA(x + 1, y + 1, z, r, g, b);\
			MODEL_SET_DATA(x + 1, y + 1, z + 1, r, g, b);\
			MODEL_SET_DATA(x + 1, y, z + 1, r, g, b);\
		}\
	}\
	else if((data) & 0xFF){\
		GET_VOXEL_RGB(data);\
		MODEL_SET_DATA(x, y, z, r, g, b);\
		MODEL_SET_DATA(x + 1, y, z, r, g, b);\
		MODEL_SET_DATA(x + 1, y + 1, z, r, g, b);\
		MODEL_SET_DATA(x, y + 1, z, r, g, b);\
\
		MODEL_SET_DATA(x, y, z + 1, r, g, b);\
		MODEL_SET_DATA(x + 1, y, z + 1, r, g, b);\
		MODEL_SET_DATA(x + 1, y + 1, z + 1, r, g, b);\
		MODEL_SET_DATA(x, y + 1, z + 1, r, g, b);\
\
		MODEL_SET_DATA(x, y, z, r, g, b);\
		MODEL_SET_DATA(x + 1, y, z, r, g, b);\
		MODEL_SET_DATA(x + 1, y, z + 1, r, g, b);\
		MODEL_SET_DATA(x, y, z + 1, r, g, b);\
\
		MODEL_SET_DATA(x, y + 1, z, r, g, b);\
		MODEL_SET_DATA(x + 1, y + 1, z, r, g, b);\
		MODEL_SET_DATA(x + 1, y + 1, z + 1, r, g, b);\
		MODEL_SET_DATA(x, y + 1, z + 1, r, g, b);\
\
		MODEL_SET_DATA(x, y, z, r, g, b);\
		MODEL_SET_DATA(x, y + 1, z, r, g, b);\
		MODEL_SET_DATA(x, y + 1, z + 1, r, g, b);\
		MODEL_SET_DATA(x, y, z + 1, r, g, b);\
\
		MODEL_SET_DATA(x + 1, y, z, r, g, b);\
		MODEL_SET_DATA(x + 1, y + 1, z, r, g, b);\
		MODEL_SET_DATA(x + 1, y + 1, z + 1, r, g, b);\
		MODEL_SET_DATA(x + 1, y, z + 1, r, g, b);\
	}\
}

render_obj_list read_qb_vxl(FILE* fd)
{
	qb_hdr hdr;
	fread(&hdr, sizeof(hdr), 1, fd);

	render_obj_list obj_list = render_obj_list_create_empty();

	for(unsigned i = 0; i < hdr.matrix_cnt; ++i){
		READ_VAR(uint8_t, name_ln);
		char* name = malloc(name_ln + 1);
		fread(name, name_ln, 1, fd);
		name[name_ln] = '\0';
		free(name); // not used (yet)

		#pragma pack(push, 1)
		struct {
			uint32_t size_x, size_y, size_z;
			int32_t pos_x, pos_y, pos_z;
		} m_info;
		#pragma pack(pop)
		fread(&m_info, sizeof(m_info), 1, fd);
		size_t voxel_cnt_max = m_info.size_x * m_info.size_y * m_info.size_z;

		// preemptively allocate space for a fraction of possible voxels, it'll be realloc'd afterwards
		size_t model_ln = voxel_cnt_max * 6 * 12 / VOXEL_ALLOC_FRACTION;
		size_t model_idx = 0;
		GLfloat* model_vert = malloc(sizeof(GLfloat) * model_ln);
		GLfloat* model_clr = malloc(sizeof(GLfloat) * model_ln);

		switch(hdr.compression){
			case COMPRESSION_NONE:
				uint32_t* data = malloc(voxel_cnt_max * sizeof(uint32_t));
				fread(data, sizeof(uint32_t), voxel_cnt_max, fd);
				for(long long z = 0; z < m_info.size_z; ++z)
					for(long long y = 0; y < m_info.size_y; ++y)
						for(long long x = 0; x < m_info.size_x; ++x)
							SET_VOXEL_DATA(data[z * m_info.size_x * m_info.size_y + y * m_info.size_x + x], x, y, z);
				free(data);
				break;
			case COMPRESSION_RLE:
				for(long long z = 0; z < m_info.size_z; ++z){
					size_t idx = 0;
					for(;;){
						READ_VAR(uint32_t, data);
						if(data == FLAG_NEXT_SLICE)
							break;
						else if(data == FLAG_CODE){
							READ_VAR(uint32_t, cnt);
							READ_VAR(uint32_t, data);
							for(size_t j = 0; j < cnt; ++j){
								long long x = idx % m_info.size_x + 1;
								long long y = idx / m_info.size_x + 1;
								++idx;
								SET_VOXEL_DATA(data, x, y, z);
							}
						}
						else{
							long long x = idx % m_info.size_x + 1;
							long long y = idx / m_info.size_x + 1;
							++idx;
							SET_VOXEL_DATA(data, x, y, z);
						}
					}
				}
				break;
		}

		model_vert = realloc(model_vert, sizeof(GLfloat) * model_idx);
		model_clr = realloc(model_clr, sizeof(GLfloat) * model_idx);
		render_obj_list_add(&obj_list,
					render_obj_create(GL_QUADS, 0, model_vert, model_idx * sizeof(GLfloat),
						RENDER_OBJ_COLOR, model_clr, model_idx * sizeof(GLfloat),
						RENDER_OBJ_ATTRIBUTES_END));
		free(model_vert); free(model_clr);
	}

	return obj_list;
}
