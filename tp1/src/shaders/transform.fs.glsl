#version 330 core

// TODO: La couleur des fragments est donnée à partir de la couleur
//       des vertices passée en entrée.
//       De plus, une variable uniform permet de multiplier la couleur
//       par une autre pour coloriser les fragments.
in vec4 vertexColor;
out vec4 fragColor;

uniform vec3 colorMod;

void main()
{
    fragColor = vertexColor*vec4(colorMod,1.0);
}
