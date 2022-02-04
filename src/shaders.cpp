#include "blkhole/shaders.hpp"

#include <GL/glew.h>
#include <GL/glut.h>

#include <cstdio>

// load shader content into memory + return. remember to dealloc
char * load_shader(const char *fname) {
    FILE *fp = fopen(fname, "r");
    if (fp == NULL)
        return 0;
    int size;
    char *content;
    if (fseek(fp, 0L, SEEK_END) == 0) {
        size = ftell(fp);
        content = (char *) malloc(size * sizeof(char) + 1);
        rewind(fp);
        fread(content, sizeof(char), size, fp);
        content[size] = '\0';
        return content;
    } else {
        return 0;
    }
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

GLuint init_compute_shader(const char *computename) {
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
    GLuint program = glCreateProgram();
    if (program == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    char *content = load_shader(computename);

    if (content == 0) {
        fprintf(stderr, "Failed to load compute shader %s\n", computename);
        exit(1);
    }

    add_shader(program, content, GL_COMPUTE_SHADER);

    GLint success = 0;
    GLchar error[1024] = { 0 };

	// After compiling all shader objects and attaching them to the program, we can finally link it
    glLinkProgram(program);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success == 0) {
		glGetProgramInfoLog(program, sizeof(error), NULL, error);
		fprintf(stderr, "Error linking shader program: '%s'\n", error);
        exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
    glValidateProgram(program);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, sizeof(error), NULL, error);
        fprintf(stderr, "Invalid shader program: '%s'\n", error);
        exit(1);
    }
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
    //glUseProgram(0);

    // dealloc
    free(content);

	return program;
}

GLuint init_shader_pair(const char *vertname, const char *fragname) {
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
    GLuint program = glCreateProgram();
    if (program == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

	// Create two shader objects, one for the vertex, and one for the fragment shader
    char *vert_content = load_shader(vertname);
    char *frag_content = load_shader(fragname);

    if (vert_content == 0) {
        fprintf(stderr, "Failed to load vertex shader %s\n", vertname);
        exit(1);
    }
    if (frag_content == 0) {
        fprintf(stderr, "Failed to load fragment shader %s\n", fragname);
        exit(1);
    }

    //printf("Loaded vertex shader: %s\n", vert_content);
    //printf("Loaded fragment shader: %s\n", frag_content);

    add_shader(program, vert_content, GL_VERTEX_SHADER);
    add_shader(program, frag_content, GL_FRAGMENT_SHADER);

    GLint success = 0;
    GLchar error[1024] = { 0 };

	// After compiling all shader objects and attaching them to the program, we can finally link it
    glLinkProgram(program);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success == 0) {
		glGetProgramInfoLog(program, sizeof(error), NULL, error);
		fprintf(stderr, "Error linking shader program: '%s'\n", error);
        exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
    glValidateProgram(program);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, sizeof(error), NULL, error);
        fprintf(stderr, "Invalid shader program: '%s'\n", error);
        exit(1);
    }
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
    //glUseProgram(program);

    // dealloc
    free(vert_content);
    free(frag_content);

	return program;
}
