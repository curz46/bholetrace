#include "blkhole/shaders.hpp"

#include <GL/glew.h>
#include <GL/glut.h>

#include <cstdio>

char * load_shader(char *fname) {
    return 0;
}

void add_shader(GLuint program, const char* text, GLenum type) {
	// create a shader object
    GLuint obj = glCreateShader(type);

    if (obj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", type);
        exit(0);
    }
	// Bind the source code to the shader, this happens before compilation
	glShaderSource(obj, 1, (const GLchar**)&text, NULL);
	// compile the shader and check for errors
    glCompileShader(obj);
    GLint success;
	// check for shader related errors using glGetShaderiv
    glGetShaderiv(obj, GL_COMPILE_STATUS, &success);
    if (! success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(obj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", type, InfoLog);
        exit(1);
    }
	// Attach the compiled shader object to the program object
    glAttachShader(program, obj);
}

GLuint init_shader(const char *shadername) {
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
    GLuint shaderProgramID = glCreateProgram();
    if (shaderProgramID == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

	// Create two shader objects, one for the vertex, and one for the fragment shader
    //AddShader(shaderProgramID, pVS, GL_VERTEX_SHADER);
    //AddShader(shaderProgramID, pFS, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };


	// After compiling all shader objects and attaching them to the program, we can finally link it
    glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
    glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
    glUseProgram(shaderProgramID);
	return shaderProgramID;
}
