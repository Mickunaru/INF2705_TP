#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;


in ATTRIB_VS_OUT
{
    vec3 position;
    float zOrientation;
    vec4 color;
    float size;
} attribIn[];

out ATTRIB_GS_OUT
{
    vec2 texCoord;
    vec4 color;
} attribOut;

uniform mat4 projection;
uniform mat4 view;

void main()
{

    // TODO: Convertiser les points en entrée en carrées.
    //       Un côté est de la taille particle::size.
    //       Les carrées sont toujours orientés vers la caméra et tourne
    //       sur eux même sur l'axe des z.
    //       Les coordonnées de texture peuvent être généré à partir de la position du point.
    //
    //       Il est important d'écrire chaque attribut de sortie du shader de géometrie,
    //       autrement c'est un comportement non défini.

    vec3 center = attribIn[0].position;

    float size = attribIn[0].size;
    float angle = attribIn[0].zOrientation;

    float halfSize = size * 0.5;

    vec2 offsets[4] = vec2[](
        vec2(-halfSize, -halfSize),
        vec2( halfSize, -halfSize),
        vec2(-halfSize,  halfSize),
        vec2( halfSize,  halfSize)
    );

    vec2 uvs[4] = vec2[](
        vec2(0.0, 0.0),
        vec2(1.0, 0.0),
        vec2(0.0, 1.0),
        vec2(1.0, 1.0)
    );

    float c = cos(angle);
    float s = sin(angle);

    vec3 camRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camUp  = vec3(view[0][1], view[1][1], view[2][1]);

    for (int i = 0; i < 4; i++)
    {
        vec2 rotated = vec2(
            offsets[i].x * c - offsets[i].y * s,
            offsets[i].x * s + offsets[i].y * c
        );

        vec3 worldPos = center + camRight * rotated.x + camUp * rotated.y;
        gl_Position = projection * vec4(worldPos, 1.0);

        attribOut.texCoord = uvs[i];
        attribOut.color = attribIn[0].color;

        EmitVertex();
    }

    EndPrimitive();
}
