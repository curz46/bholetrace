#version 430 core
layout (location = 0) in vec2 inPos; // the position variable has attribute position 0
layout (location = 1) in vec2 inTexCoord;
  
out vec2 texCoord; 

void main()
{
    gl_Position = vec4(inPos, 1.0f, 1.0); 
    texCoord = inTexCoord;
}
