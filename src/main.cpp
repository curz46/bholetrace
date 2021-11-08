#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>

#include "blkhole/shaders.hpp"

// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;

GLuint vao;
GLuint vbo;

void display() {
    while(1) {
        glClear(GL_COLOR_BUFFER_BIT);
        // NB: Make the call to draw the geometry in the currently activated vertex buffer. This is where the GPU starts to work!
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glutSwapBuffers();
    }
}

void init() {
    // TODO: figure out how to bundle into the executable
    init_shader_pair("src/shader/render.vert", "src/shader/render.frag");

    //GLfloat vertices[] = {
    //    -0.5f, -0.5f, 0.0f,
    //    0.5f, -0.5f, 0.0f,
    //    0.0f,  0.5f, 0.0f
    //};  
    GLfloat vertices[] = {
        -1.f, -1.f,
        -1.f, 1.f,
        1.f, 1.f,
        1.f, 1.f,
        1.f, -1.f,
        -1.f, -1.f
    };
	// set vao, vbo globals
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main(int argc, char** argv){
	// Set up the window
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Black Hole Trace");

	 // A call to glewInit() must be done after glut is initialized!
    GLenum res = glewInit();
	// Check for any errors
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1;
    }
	// Set up your objects and shaders
	init();
	// Tell glut where the display function is
	glutDisplayFunc(display);

	// Begin infinite event loop
	glutMainLoop();
    return 0;
}
