#version 420 core
#extension GL_ARB_explicit_uniform_location : require

in vec2 texCoord; 

layout (location = 1) uniform sampler2D sampler;

out vec4 fragColor;

void main()
{
    fragColor = texture(sampler, texCoord);
    //fragColor.r = 1.f;
} 
