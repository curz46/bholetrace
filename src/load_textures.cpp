#include <cstring>
#include <cstdio>

#include <GL/glut.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "bholetrace/load_textures.hpp"

/*
 * Load cube map textures into OpenGL
 *
 * See bholetrace/load_textures.h for a full definition.
 */
int load_cubemap(int texid, char *fprefix, char *ext) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, texid);
    
    int width,height,n;
    char fname[100] = "\0";
    strcat(fname, fprefix);
    strcat(fname, "-i");
    int ipos = strlen(fprefix) + 1;
    for (int i = 0; i < 6; i++) {
        sprintf(&fname[ipos], "%d%s", i, ext);
        //debug
        printf("load_cubemap: Loading index=%d, fname=%s into cube map\n", i, fname);
        unsigned char *data = stbi_load(fname, &width, &height, &n, 3/*rgb*/);
        glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    //debug
    printf("load_cubemap: Done\n");
    return 0;
}
