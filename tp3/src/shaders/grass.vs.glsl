#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 mvp;

out ATTRIBS_VS_OUT {
    vec3 worldPos;
} attribsOut;

void main()
{
    attribsOut.worldPos = position;
    attribsOut.worldPos.y = 0.01;

    gl_Position =  vec4(position, 1.0);
    gl_Position.y = 0.01;
}
