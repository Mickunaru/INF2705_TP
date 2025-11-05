#version 330 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;

uniform mat4 mvp;

void main()
{
    vec3 newPosition = position + normal * 0.05;
    gl_Position = mvp * vec4(newPosition, 1.0);
}
