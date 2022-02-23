#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/vec3.hpp>

#include "bholetrace/load_shaders.hpp"
#include "bholetrace/load_textures.hpp"
#include "bholetrace/keys.hpp"

using namespace std;

const int TEXTURE_WIDTH = 400;
const int TEXTURE_HEIGHT = 225;

int ticks = 0;

struct Camera {
    glm::vec3 pos;
    glm::vec3 rot;
    float fov;
} camera;

// The texture used for the compute shader to store its results
GLuint transfer_tex;
// The skybox texture (cube starmap)
GLuint skybox_tex;

// The program which computes the raycasts
GLuint compute_program;
// The (trivial) program which renders the resultant texture to the screen
GLuint shader_program;

// Vertex Array Object, Vertex Buffer Object
GLuint vao;
GLuint vbo;

static void on_key_press(unsigned char key, int _x, int _y) {
    key_toggle(key, 1, ticks);
    printf("on_key_press: %c\n", key);
}

static void on_key_release(unsigned char key, int _x, int _y) {
    key_toggle(key, 0, ticks);
    printf("on_key_release: %c\n", key);
}

static void update() {
    ticks++;

    //printf("Update!\n");

    if (key_pressed('a')) {
        camera.rot.x += 5;
        printf("Rotated camera on +rotX.\n");
    } else if (key_pressed('d')) {
        camera.rot.x -= 5;
        printf("Rotated camera on -rotX.\n");
    }
}

void display() {
    while(1) {
        // COMPUTE
        glUseProgram(compute_program);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

		glUniform2f(0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
        glUniform1i(glGetUniformLocation(shader_program, "destTex"), transfer_tex);
        glUniform1i(glGetUniformLocation(shader_program, "skybox"), skybox_tex);

		glDispatchCompute(TEXTURE_WIDTH, TEXTURE_HEIGHT, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // SHADER
        glUseProgram(shader_program);

        //glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindVertexArray(vao);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glutSwapBuffers();

        update();
    }
}

void init() {
    // TODO: figure out how to bundle into the executable
    compute_program = init_compute_shader("shaders/raycast.comp");
    shader_program  = init_shader_pair("shaders/render.vert", "shaders/render.frag");

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

    //generate transfer texture
    glGenTextures(1, &transfer_tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, transfer_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(0, transfer_tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    //load skybox texture
    glGenTextures(1, &skybox_tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_tex);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    load_cubemap(skybox_tex, "textures/starmap", ".png");

    //setup camera
    camera.pos = glm::vec3(-100., 0., 0.);
    camera.rot = glm::vec3(0., 0., 0.);
    camera.fov = 100.;
}

int main(int argc, char** argv){
	// Set up the window
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Black Hole Trace");

    glutKeyboardFunc(on_key_press);
    glutKeyboardUpFunc(on_key_release);

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
