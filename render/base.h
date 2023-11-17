#ifndef RENDER_BASE_H
#define RENDER_BASE_H

#define GL_GLEXT_PROTOTYPES 1
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string.h>
#include "more_math.h"

extern GLFWwindow* main_wnd;

extern vec3f render_cam_pos;
extern vec3f render_cam_rot;

int render_init();
void render_loop();

/* Bounding box */
void render_push_bbox(vec2f pos, vec2f size);
void render_pop_bbox();

/* Render object */
#define RENDER_OBJ_ATTRIBUTES_END	0xFFFF

// Attribute type		Dimensionality		Type
// vertex			3			GLfloat
// color			3			GLfloat
// bindless texture handle	1			GLuint (as a depth index in render/shader.c 3D atlas texture)
// texture coords		2			GLfloat

// Flags determine what attributes are included in render objects.
// Vertices are included in every render object.
#define RENDER_OBJ_VERTS	0
#define RENDER_OBJ_COLOR	1
#define RENDER_OBJ_TEXTURE	2
#define RENDER_OBJ_TEX_COORDS	3

#define RENDER_OBJ_ATTRIBUTES_COUNT	4

// Other flags that do not contribute to attribute array
#define RENDER_OBJ_MAIN_TEXTURE		100
#define RENDER_OBJ_COLORIZE		101

// Render object flags
#define RENDER_OBJ_FLAG_INVALID		0x1	// signals that function returning a render object failed

typedef struct render_obj render_obj;
struct render_obj {
	int flags;

	vec3f colorize;
	int render_type;

	GLuint buf;
	GLuint main_tex;
	size_t buf_sizes[RENDER_OBJ_ATTRIBUTES_COUNT];
};
#define RENDER_OBJ_EMPTY ((render_obj){.buf = 0})

typedef struct render_obj_dynamic render_obj_dynamic;
struct render_obj_dynamic {
	struct render_obj;
	void* attr_data[RENDER_OBJ_ATTRIBUTES_COUNT];
};

/* Other types of attributes are supplied through varargs in following fashion:
 * [int] attribute type (RENDER_OBJ_COLOR, ...),
 * [void*] attribute data
 * [size_t] attribute length
 * !!! The end of attribute list is marked with RENDER_OBJ_ATTRIBUTES_END !!!
 */
render_obj render_obj_create(int render_type, int flags, GLfloat* verts, size_t verts_ln, ...);
render_obj_dynamic render_obj_create_dynamic(int render_type, int flags, GLfloat* verts, size_t verts_ln, ...);

void render_obj_draw(render_obj* obj);
void render_obj_free(render_obj* obj);

typedef struct {
	render_obj* objs;
	size_t ln;
} render_obj_list;

render_obj_list render_obj_list_create_empty();
void render_obj_list_free(render_obj_list* list);
void render_obj_list_add(render_obj_list* list, render_obj obj);
void render_obj_list_draw(render_obj_list* list); 

#endif
