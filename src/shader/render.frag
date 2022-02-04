#version 330 core
in vec2 texCoord; 

uniform sampler2D sampler;

void main()
{
    gl_FragColor = texture(sampler, texCoord);
    //gl_FragColor.r = 1.f;
} 
