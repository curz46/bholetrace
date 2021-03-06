#include <iostream>
#include <math.h>
#include <sys/time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

#include "bholetrace/load_shaders.hpp"
#include "bholetrace/load_textures.hpp"
#include "bholetrace/keys.hpp"

using namespace std;

const int WINDOW_WIDTH  = 1920;
const int WINDOW_HEIGHT = 1080;

const int RAYCAST_WIDTH = 1920/2;
const int RAYCAST_HEIGHT = 1080/2;

static GLFWwindow *window;
static int ticks = 0;

static struct Camera {
    glm::vec3 pos;
    glm::vec3 rot;
    float fov;
} camera;
static glm::vec3 user_rot;
static float orbit_radius = 25.;
static int cubemap = 0;
static int rotate = 2;
static bool deflection = true;

// The texture used for the compute shader to store its results
static GLuint transfer_tex;
// The skybox texture (cube starmap)
static GLuint skybox_tex;

// The program which computes the raycasts
static GLuint compute_program;
// The (trivial) program which renders the resultant texture to the screen
static GLuint shader_program;

// Vertex Array Object, Vertex Buffer Object
static GLuint vao;
static GLuint vbo;

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{                                                                               
    if (action == GLFW_PRESS)                                                   
        key_toggle(key, 1, ticks);                                              
    else if (action == GLFW_RELEASE)                                            
        key_toggle(key, 0, ticks);                                              
}

static void update() {
    if (key_pressed_tick(GLFW_KEY_Z, ticks)) {
        cubemap = (cubemap + 1) % 3;
        if (cubemap == 0)
            load_cubemap(skybox_tex, "textures/nebula", ".png");
        else if (cubemap == 1)
            load_cubemap(skybox_tex, "textures/starmap", ".png");
        else if (cubemap == 2)
            load_cubemap(skybox_tex, "textures/grid", ".jpg");
    }
    if (key_pressed_tick(GLFW_KEY_R, ticks)) {
        switch (rotate) {
            case 4: rotate=2; break;
            case 2: rotate=1; break;
            case 1: rotate=0; break;
            case 0: rotate=4; break;
        }
    }
    if (key_pressed_tick(GLFW_KEY_L, ticks)) {
        deflection=!deflection;
    }
    if (key_pressed(GLFW_KEY_A)) {
        user_rot.y += 0.75;
    }
    if (key_pressed(GLFW_KEY_D)) {
        user_rot.y -= 0.75;
    }
    if (key_pressed(GLFW_KEY_W)) {
        user_rot.x += 0.75;
    }
    if (key_pressed(GLFW_KEY_S)) {
        user_rot.x -= 0.75;
    }
    //log here makes us approach blackhole slower as we get closer
    if (key_pressed(GLFW_KEY_DOWN)) {
        orbit_radius -= 0.05*std::max(0.f,(float)pow(log(orbit_radius),3));
    }
    if (key_pressed(GLFW_KEY_UP)) {
        orbit_radius += 0.05*std::max(0.1f,(float)pow(log(orbit_radius),3));
    }
    ticks++;
}

long long micro_now() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000000ll + now.tv_usec;
}

long long millis_now() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000ll + now.tv_usec / 1000;
}

void loop() {
    long long last_update = 0l;
    long long last_second = 0l;
    long long last_avg = micro_now();

    float t = 0.;
    int fps = 0;
    int frames = 0;
    
    int fps_low = -1;
    const int avg_interval = 1000;

    while (! glfwWindowShouldClose(window)) {
        long long millis = millis_now();
        if ((millis - 1000/60) >= last_update) {
            last_update = millis;
            update(); 
            ticks++;
        }
        // track fps
        frames++;
        fps++;
        if ((millis - 1000) >= last_second) {
            printf("%d frames per second\n", fps);
            fps=0;
            last_second = millis;

            if (fps_low == -1 || fps_low>fps)
                fps_low = fps;
        }
        // track average fps
        if (frames>0 && frames % avg_interval == 0) {
            long long mnow = micro_now();
            long long elapsed = mnow - last_avg;
            float avg_fps = avg_interval / (elapsed/(float)1000000);
            float frame_time = elapsed / (float)avg_interval;
            float T_total = frame_time / (float)(RAYCAST_WIDTH*RAYCAST_HEIGHT);
            printf("average fps: %f, average frame time: %f us, average ray time: %f us\n",
                    avg_fps, frame_time, T_total);
            last_avg = mnow;
        }

        //orbit oscillation for consistent testing
        //orbit_radius = 20 + 17.5 * sin((millis/10 % 360) * M_PI/180);
        camera.pos = glm::vec3(orbit_radius * sin(t*M_PI/180), 0., orbit_radius * cos(t*M_PI/180));
        camera.rot = glm::vec3(0., t+180, 0.) + user_rot;
        t += 0.05*rotate;

        // COMPUTE
        glUseProgram(compute_program);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

		glUniform2f(glGetUniformLocation(compute_program, "size"), RAYCAST_WIDTH, RAYCAST_HEIGHT);
        glUniform1i(glGetUniformLocation(compute_program, "skybox"), 1);

        //camera
        glUniform3f(glGetUniformLocation(compute_program, "camera.pos"), camera.pos.x, camera.pos.y, camera.pos.z);
        glUniform3f(glGetUniformLocation(compute_program, "camera.rot"), camera.rot.x, camera.rot.y, camera.rot.z);
        glUniform1f(glGetUniformLocation(compute_program, "camera.fov"), camera.fov);

        //deflection
        glUniform1i(glGetUniformLocation(compute_program, "deflection"), deflection);

        //perform raytracing
		glDispatchCompute(RAYCAST_WIDTH, RAYCAST_HEIGHT, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // SHADER
        glUseProgram(shader_program);

        glUniform1i(glGetUniformLocation(shader_program, "sampler"), 0);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindVertexArray(vao);

        glClear(GL_COLOR_BUFFER_BIT);
        //render to screen
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void init() {
    // TODO: figure out how to bundle into the executable
    compute_program = init_compute_shader("shaders/raycast.comp");
    shader_program  = init_shader_pair("shaders/render.vert", "shaders/render.frag");

    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP);

    GLfloat vertices[] = {
        -1.f, -1.f, 0.f, 0.f,
        -1.f, 1.f, 0.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, -1.f, 1.f, 0.f,
        -1.f, -1.f, 0.f, 0.f
    };
	// set vao, vbo globals
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

    //generate transfer texture
    glGenTextures(1, &transfer_tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, transfer_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, RAYCAST_WIDTH, RAYCAST_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
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

    //load_cubemap(skybox_tex, "textures/starmap", ".png");
    //load_cubemap(skybox_tex, "textures/grid", ".jpg");
    load_cubemap(skybox_tex, "textures/nebula", ".png");

    //setup camera
    camera.pos = glm::vec3(-25., 0., 0.);
    camera.rot = glm::vec3(0., 0., 0.);
    camera.fov = 90.;
}

int main(int argc, char** argv) {
    glfwSetErrorCallback(error_callback);

    if (! glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "BHOLETRACE", NULL, NULL);
    if (! window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    GLenum res = glewInit();
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1;
    }

	init();
	loop();

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);

    return 0;
}
