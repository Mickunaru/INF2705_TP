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
    // TODO: Colorier les particules.
    //       La couleur finale est la couleur de la texture teinté
    //       par la couleur de la particule.
    //       Pour éviter de traiter des fragments invisibles, on discard
    //       les fragments dont le texel possède moins de 0.02 d'opacité.

    vec4 texColor = texture(textureSampler, attribIn.texCoord);

    FragColor = texColor * attribIn.color;

    if (FragColor.a < 0.02)
        discard;
}
