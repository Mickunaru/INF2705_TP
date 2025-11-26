#version 330 core

in ATTRIB_GS_OUT
{
    vec2 texCoord;
    vec4 color;
} attribIn;

out vec4 FragColor;

uniform sampler2D textureSampler;

void main()
{
    vec4 texColor = texture(textureSampler, attribIn.texCoord);

    FragColor = texColor * attribIn.color;

    if (FragColor.a < 0.02)
        discard;
}
