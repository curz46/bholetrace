#ifndef BLKHOLE_SHADERS__H
#define BLKHOLE_SHADERS__H

#include <GL/glew.h>
#include <GL/glut.h>

char * load_shader(char *fname);

void add_shader(GLuint program, const char *text, GLenum type);

GLuint compile_shaders();

#endif // BLKHOLE_SHADERS__H
