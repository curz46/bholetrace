#ifndef BLKHOLE_SHADERS__H
#define BLKHOLE_SHADERS__H

#include <GL/glew.h>
#include <GL/glut.h>

char * load_shader(const char *fname);

void add_shader(GLuint program, const char *text, GLenum type);

GLuint init_compute_shader(const char *computename);

GLuint init_shader_pair(const char *vertname, const char *fragname);

#endif // BLKHOLE_SHADERS__H
