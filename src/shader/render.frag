#version 330 core
//out vec4 FragColor;
  
in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  

void main()
{
    gl_FragColor = vec4(1.f, 1.f, 1.f, 1.f);
} 
