#ifndef LOAD_TEXTURES__H
#define LOAD_TEXTURES__H

/*
 * load_cubemap expects the filename to be in the format `{fprefix}-i` where
 * i is the index of the texture to be placed in the cube map.
 *
 * Texture target	                Orientation     Index
 * GL_TEXTURE_CUBE_MAP_POSITIVE_X   Right           0
 * GL_TEXTURE_CUBE_MAP_NEGATIVE_X	Left            1
 * GL_TEXTURE_CUBE_MAP_POSITIVE_Y	Top             2
 * GL_TEXTURE_CUBE_MAP_NEGATIVE_Y	Bottom          3
 * GL_TEXTURE_CUBE_MAP_POSITIVE_Z	Back            4
 * GL_TEXTURE_CUBE_MAP_NEGATIVE_Z	Front           5
 *
 * This function will bind `texid` to the active OpenGL texture.
 */
int load_cubemap(int texid, char *fprefix, char *ext);

#endif // LOAD_TEXTURES__H
