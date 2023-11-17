#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

#include "render/base.h"
#include "more_math.h"
#include "resources.h"

// rendering
render_obj render_atlas_texture(atlas_texture* tex, vec2f coords, vec2f size);
render_obj render_atlas_textures(atlas_texture** arr_tex, vec2f* arr_coords, vec2f* arr_size,
					size_t n);

// helper functions
float atlas_texture_aspect_w(atlas_texture* tex, float height);
float atlas_texture_aspect_h(atlas_texture* tex, float width);

#endif
