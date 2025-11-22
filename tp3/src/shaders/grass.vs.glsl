#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 mvp;
//uniform mat4 modelView;

out ATTRIBS_VS_OUT {
    vec3 worldPos;
} attribsOut;

void main()
{
    attribsOut.worldPos = position;
    gl_Position =  vec4(position, 1.0);
}
