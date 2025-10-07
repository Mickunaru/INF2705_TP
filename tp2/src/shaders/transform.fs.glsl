#version 330 core

uniform vec3 colorMod;

in vec4 vertexColor;

out vec4 fragColor;

void main()
{
    fragColor = vec4(vertexColor.rgb * colorMod, vertexColor.a);
}
