#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H

#include <stdio.h>

void read_texture_atlas(FILE* fd);
void load_texture_atlases(); // loads all previously read texture atlases into GPU memory as a 3D texture and resizes texture coordinates for atlas textures to fit this 3D texture. SHOULD BE CALLED ONLY ONCE.

#endif
