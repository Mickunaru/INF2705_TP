#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in float zOrientation;
layout(location = 2) in vec4 color;
layout(location = 3) in float size;

out ATTRIB_VS_OUT
{
    vec3 position;
    float zOrientation;
    vec4 color;
    float size;
} attribOut;

uniform mat4 modelView;

void main()
{
    gl_Position = modelView * vec4(position, 1.0);
    attribOut.position = position;
    attribOut.color = color;
    attribOut.size = size;
    attribOut.zOrientation = zOrientation;
}
