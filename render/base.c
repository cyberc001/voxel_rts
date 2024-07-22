#include "render/base.h"
#include <stdio.h>
#include <stdarg.h>

#include "render/terrain.h"
#include "render/shader.h"

#include "more_math.h"
#include "ticker.h"
#include "resources.h"

#include "game/logic.h"
#include "render/list.h"

// TEST
#include "controls.h"

GLFWwindow* main_wnd;
static vec2i wnd_shape = {0, 0};

mat4f last_mview_mat = mat4f_identity();
mat4f last_proj_mat = mat4f_identity();

vec3f render_cam_pos = {4.07, 12.2, -3.93};
vec3f render_cam_rot = {40, 180, 0};

static GLuint __dummy_tex; // 1x1 texture that signals that main texture is not in use

void glfwPerspective(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{ // https://stackoverflow.com/questions/12943164/replacement-for-gluperspective-with-glfrustrum
	double fH = tan(fovY / 360 * M_PI) * zNear;
	double fW = fH * aspect;
 	glFrustum( -fW, fW, -fH, fH, zNear, zFar );
}

static void render_display()
{
	glViewport(0, 0, wnd_shape.x, wnd_shape.y);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Game world rendering
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glfwPerspective(70, wnd_shape.x / wnd_shape.y, 1, 100);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(render_cam_rot.x, 1, 0, 0);
	glRotatef(render_cam_rot.y, 0, 1, 0);
	glRotatef(render_cam_rot.z, 0, 0, 1);
	glTranslatef(-render_cam_pos.x, -render_cam_pos.y, -render_cam_pos.z);

	GLfloat modelview_mat[16];
	GLfloat projection_mat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_mat);
	glGetFloatv(GL_PROJECTION_MATRIX, projection_mat);
	last_mview_mat = mat4f_from_gl(modelview_mat);
	last_proj_mat = mat4f_from_gl(projection_mat);
	
	render_terrain();

	vec2f mouse = get_mouse_coords();
	TRANSLATE_ORTHO_COORDS(mouse);

	vec3f proj = vec2f_project3(mouse);

	proj = vec3_norm(proj);
	proj = vec3_smul(proj, 100);

	vec3f start = get_camera_center(),
	      end = vec3_add(render_cam_pos, proj);

	float verts[] = {
		start.x, start.y, start.z,
		end.x, end.y, end.z
	};
	render_obj ro = render_obj_create(GL_LINES, 0, verts, sizeof(verts), RENDER_OBJ_ATTRIBUTES_END);
	render_obj_draw(&ro);
	render_obj_free(&ro);

	render_list_swap_front_and_mid();
	for(size_t i = 0; i < render_list_front->draw.busy; ++i){
		render_info_draw inf = render_list_front->draw.data[i];

		if(inf.robj->flags & RENDER_OBJ_FLAG_NOTINIT)
			render_obj_load(inf.robj);

		glGetFloatv(GL_MODELVIEW_MATRIX, modelview_mat); // idk why glPushMatrix / glPopMatrix don't work
		glTranslatef(inf.tr.x, inf.tr.y, inf.tr.z);
		mat4f rot_mat = mat_from_quat(inf.rot);
		GLfloat trmatf[] = { rot_mat.e[0][0], rot_mat.e[0][1], rot_mat.e[0][2], rot_mat.e[0][3],
				rot_mat.e[1][0], rot_mat.e[1][1], rot_mat.e[1][2], rot_mat.e[1][3],
				rot_mat.e[2][0], rot_mat.e[2][1], rot_mat.e[2][2], rot_mat.e[2][3],
				rot_mat.e[3][0], rot_mat.e[3][1], rot_mat.e[3][2], rot_mat.e[3][3],
		};
		glScalef(inf.sc.x, inf.sc.y, inf.sc.z);
		glMultMatrixf(trmatf);

		render_obj_draw(inf.robj);
		glLoadMatrixf(modelview_mat);
	}
	for(size_t i = 0; i < render_list_front->free.busy; ++i){
		render_info_free inf = render_list_front->free.data[i];
		render_obj_free(inf.robj);
		free(inf.robj);
	}
	render_list_front->free.busy = 0;

	// UI rendering
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST); 
	// glOrtho was here for some reason and it caused occasional flickering. This saved my day: https://stackoverflow.com/questions/43837846/can-i-use-a-second-opengl-context-to-render-a-ui-on-top-of-a-3d-scene

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	ui_render();

	glfwSwapBuffers(main_wnd);
}
static void render_reshape(GLFWwindow* wnd, int w, int h)
{
	wnd_shape.x = w; wnd_shape.y = h;
}

/* Bounding box */
static rect2f bbox_stack[32]; static int bbox_stack_top = 0;

void render_push_bbox(vec2f pos, vec2f size)
{
	rect2f prev_top = (bbox_stack_top == 0 ? (rect2f){{0, 0}, {1, 1}} : bbox_stack[bbox_stack_top - 1]);
	rect2f top = {pos, size};
	top.pos = (vec2f){max(top.pos.x, prev_top.pos.x), max(top.pos.y, prev_top.pos.y)};
	vec2f min_rt = {min(top.pos.x + top.size.x, prev_top.pos.x + prev_top.size.x),
			min(top.pos.y + top.size.y, prev_top.pos.y + prev_top.size.y)}; // minimum top right rectangle corner
	top.size = vec2_sub(min_rt, top.pos);
	top.size = (vec2f){max(top.size.x, 0), max(top.size.y, 0)};
	bbox_stack[bbox_stack_top++] = top;

	glScissor(top.pos.x * wnd_shape.x, top.pos.y * wnd_shape.y,
			top.size.x * wnd_shape.x, top.size.y * wnd_shape.y);
	glEnable(GL_SCISSOR_TEST);
}
void render_pop_bbox()
{
	bbox_stack_top = max(bbox_stack_top - 1, 0);
	if(bbox_stack_top == 0)
		glDisable(GL_SCISSOR_TEST);
	else {
		rect2f top = bbox_stack[bbox_stack_top - 1];
		glScissor(top.pos.x * wnd_shape.x, top.pos.y * wnd_shape.y,
			top.size.x * wnd_shape.x, top.size.y * wnd_shape.y);
	}
}

int render_init()
{
	if(!glfwInit())
		return -1;

	main_wnd = glfwCreateWindow(800, 600, "test", NULL, NULL);
	if(!main_wnd)
		return -2;
	render_reshape(main_wnd, 800, 600);
	glfwMakeContextCurrent(main_wnd);
	glfwSwapInterval(1);

	glEnable(GL_ALPHA_TEST);
	
	glfwSetWindowSizeCallback(main_wnd, render_reshape);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shader_init();

	glGenTextures(1, &__dummy_tex);
	glBindTexture(GL_TEXTURE_2D, __dummy_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1, 1, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

	render_list_init();
	return 0;
}

void render_loop()
{
	while(!glfwWindowShouldClose(main_wnd))
		render_display();
}

/* Render object */
#define __RENDER_OBJ_GENERATE()\
	render_obj o = {.render_type = render_type, .flags = flags | RENDER_OBJ_FLAG_NOTINIT, .colorize = (vec3f){-1, -1, -1}, .cut_min = (vec3f){-INFINITY, -INFINITY, -INFINITY}, .cut_max = (vec3f){INFINITY, INFINITY, INFINITY}};\
	memset(o.buf_sizes, 0, sizeof(o.buf_sizes));\
	va_list args;\
	va_start(args, verts_ln);\
	memset(o.attr_data, 0, sizeof(o.attr_data));\
	o.attr_data[RENDER_OBJ_VERTS] = verts; o.buf_sizes[RENDER_OBJ_VERTS] = verts_ln;\

#define __RENDER_OBJ_PARSE_ATTRIBUTES()\
	for(;;){\
		int type = va_arg(args, int);\
		if(type >= RENDER_OBJ_ATTRIBUTES_COUNT){\
			if(type == RENDER_OBJ_MAIN_TEXTURE){\
				o.main_tex = va_arg(args, GLuint);\
				continue;\
			}\
			else if(type == RENDER_OBJ_COLORIZE){\
				o.colorize = va_arg(args, vec3f);\
				continue;\
			}\
			else\
				break;\
		}\
		o.attr_data[type] = va_arg(args, void*);\
		o.buf_sizes[type] = va_arg(args, size_t);\
	}

render_obj render_obj_create(int render_type, int flags, GLfloat* verts, size_t verts_ln, ...)
{
	__RENDER_OBJ_GENERATE()
	__RENDER_OBJ_PARSE_ATTRIBUTES()
	render_obj_load(&o);
	return o;
}
render_obj render_obj_generate(int render_type, int flags, GLfloat* verts, size_t verts_ln, ...)
{
	__RENDER_OBJ_GENERATE()
	__RENDER_OBJ_PARSE_ATTRIBUTES()
	return o;
}
void render_obj_load(render_obj* obj)
{
	size_t buf_sz = 0;
	for(size_t i = 0; i < RENDER_OBJ_ATTRIBUTES_COUNT; ++i)
		buf_sz += obj->buf_sizes[i];
	glGenBuffers(1, &obj->buf);
	glBindBuffer(GL_ARRAY_BUFFER, obj->buf);
	glBufferData(GL_ARRAY_BUFFER, buf_sz, 0, GL_STATIC_READ);

	size_t buf_off = 0;
	for(size_t i = 0; i < RENDER_OBJ_ATTRIBUTES_COUNT; ++i){
		glBufferSubData(GL_ARRAY_BUFFER, buf_off, obj->buf_sizes[i], obj->attr_data[i]);
		buf_off += obj->buf_sizes[i];
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	obj->flags &= ~RENDER_OBJ_FLAG_NOTINIT;
	if(obj->flags & RENDER_OBJ_FLAG_ALLOCED){
		for(size_t i = 0; i < RENDER_OBJ_ATTRIBUTES_COUNT; ++i)
			if(obj->attr_data[i])
				free(obj->attr_data[i]);
		obj->flags &= ~RENDER_OBJ_FLAG_ALLOCED;
	}
}

void render_obj_draw(const render_obj* obj)
{
	size_t buf_off = obj->buf_sizes[0];

	// Main texture
	glActiveTexture(SHADER_MAIN_TEXTURE);
	if(obj->main_tex)
		glBindTexture(GL_TEXTURE_2D, obj->main_tex);
	else
		glBindTexture(GL_TEXTURE_2D, __dummy_tex);

	// Colorize
	if(obj->colorize.x >= 0)
		shader_set_colorize(obj->colorize);
	else
		shader_clear_colorize();
	// Cut
	shader_set_cut(obj->cut_min, obj->cut_max);

	glBindBuffer(GL_ARRAY_BUFFER, obj->buf);
	for(size_t i = 1; i < RENDER_OBJ_ATTRIBUTES_COUNT; ++i){
		if(!obj->buf_sizes[i])
			continue;
		switch(i){
			case RENDER_OBJ_COLOR:
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)buf_off);
				break;
			case RENDER_OBJ_TEXTURE:
				glEnableVertexAttribArray(2);
				glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, 0, (void*)buf_off);
				break;
			case RENDER_OBJ_TEX_COORDS:
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)buf_off);
				break;
		}
		buf_off += obj->buf_sizes[i];
	}

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	shader_load_gl_matrix();
	if(obj->flags & RENDER_OBJ_FLAG_WIREFRAME)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(obj->render_type, 0, obj->buf_sizes[RENDER_OBJ_VERTS] / sizeof(GLfloat) / 3);
	if(obj->flags & RENDER_OBJ_FLAG_WIREFRAME)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	for(size_t i = 1; i <= 2; ++i)
		glDisableVertexAttribArray(i);
}
void render_obj_free(render_obj* obj)
{
	glDeleteBuffers(1, &obj->buf);
	if(obj->flags & RENDER_OBJ_FLAG_ALLOCED){
		for(size_t i = 0; i < RENDER_OBJ_ATTRIBUTES_COUNT; ++i)
			if(obj->attr_data[i])
				free(obj->attr_data[i]);
	}
}
