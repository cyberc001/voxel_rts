#include "render/texture.h"

// rendering
render_obj render_atlas_texture(atlas_texture* tex, vec2f coords, vec2f size)
{
	TRANSLATE_ORTHO_COORDS(coords); TRANSLATE_ORTHO_SIZE(size);
	GLfloat verts[] = {coords.x,coords.y,0, coords.x+size.x,coords.y,0, coords.x+size.x,coords.y+size.y,0, coords.x,coords.y+size.y,0};
	GLuint tex_arr[] = {tex->atlas_layer, tex->atlas_layer, tex->atlas_layer, tex->atlas_layer};
	return render_obj_create(GL_QUADS, 0, verts, sizeof(verts), RENDER_OBJ_TEX_COORDS, tex->coords, sizeof(tex->coords), RENDER_OBJ_TEXTURE, tex_arr, sizeof(tex_arr), RENDER_OBJ_ATTRIBUTES_END);
}
render_obj render_atlas_textures(atlas_texture** arr_tex, vec2f* arr_coords, vec2f* arr_size,
					size_t n)
{
	GLfloat* verts = malloc(12*n*sizeof(GLfloat));
	GLfloat* tex_coords = malloc(8*n*sizeof(GLfloat));
	GLuint* tex_arr = malloc(4*n*sizeof(GLuint));
	for(size_t i = 0; i < n; ++i){
		vec2f coords = arr_coords[i], size = arr_size[i];
		atlas_texture* tex = arr_tex[i];
		TRANSLATE_ORTHO_COORDS(coords); TRANSLATE_ORTHO_SIZE(size);
		verts[i*12] = coords.x; verts[i*12+1] = coords.y; verts[i*12+2] = 0;
		verts[i*12+3] = coords.x+size.x; verts[i*12+4] = coords.y; verts[i*12+5] = 0;
		verts[i*12+6] = coords.x+size.x; verts[i*12+7] = coords.y+size.y; verts[i*12+8] = 0;
		verts[i*12+9] = coords.x; verts[i*12+10] = coords.y+size.y; verts[i*12+11] = 0;
		for(size_t j = 0; j < 8; ++j)
			tex_coords[i*8+j] = tex->coords[j];
		tex_arr[i*4] = tex_arr[i*4+1] = tex_arr[i*4+2] = tex_arr[i*4+3] = tex->atlas_layer;
	}
	render_obj ro = render_obj_create(GL_QUADS, 0, verts, 12*n*sizeof(GLfloat), RENDER_OBJ_TEX_COORDS, tex_coords, 8*n*sizeof(GLfloat), RENDER_OBJ_TEXTURE, tex_arr, 4*n*sizeof(GLuint), RENDER_OBJ_ATTRIBUTES_END);
	free(verts); free(tex_coords); free(tex_arr);
	return ro;
}

// helper functions
float atlas_texture_aspect_w(atlas_texture* tex, float height)
{
	float h = tex->coords[3] - tex->coords[5];
	float w = tex->coords[2] - tex->coords[0];
	return height * (w / h);
}
float atlas_texture_aspect_h(atlas_texture* tex, float width)
{
	float h = tex->coords[3] - tex->coords[5];
	float w = tex->coords[2] - tex->coords[0];
	return width * (h / w);
}
