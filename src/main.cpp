#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>

#include "blkhole/shaders.hpp"

using namespace std;

const int TEXTURE_WIDTH = 400;
const int TEXTURE_HEIGHT = 225;

// The texture used for the compute shader to store its results
GLuint texture;

// The program which computes the raycasts
GLuint compute_program;
// The (trivial) program which renders the resultant texture to the screen
GLuint shader_program;

// Vertex Array Object, Vertex Buffer Object
GLuint vao;
GLuint vbo;

void display() {
    GLuint tex;
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    while(1) {
        // COMPUTE
        glUseProgram(compute_program);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

		glUniform2f(0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
        glUniform1i(glGetUniformLocation(shader_program, "sampler"), 0);

		glDispatchCompute(TEXTURE_WIDTH, TEXTURE_HEIGHT, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // SHADER
        glUseProgram(shader_program);

        //glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindVertexArray(vao);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glutSwapBuffers();
    }
}

void init() {
    // TODO: figure out how to bundle into the executable
    compute_program = init_compute_shader("src/shader/raycast.comp");
    shader_program  = init_shader_pair("src/shader/render.vert", "src/shader/render.frag");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

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
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    //glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
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
