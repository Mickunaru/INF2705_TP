#version 330 core

in ATTRIBS_GS_OUT
{
    float height;
} attribsIn;

out vec4 FragColor;

void main()
{
    const vec3 GRASS_LIGHT = vec3(0.25, 0.35, 0.08);
    const vec3 GRASS_DARK  = vec3(0.10, 0.25, 0.03);

    float h = clamp(attribsIn.height, 0.0, 1.0);
    vec3 color = mix(GRASS_DARK, GRASS_LIGHT, h);

    FragColor = vec4(color, 1.0);
}
