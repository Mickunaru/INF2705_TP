#version 400 core

// TODO: À remplir
layout(triangles) in;

const int CLIP_COUNT = 2;

in ATTRIBS_TES_IN {
    vec3 worldPos;
    float pointSize; 
    float clipDist[CLIP_COUNT];
} attribsIn[];

out ATTRIBS_TES_OUT // Sent to geo shader
{
    vec3 worldPos;
    float pointSize;
    float clipDist[CLIP_COUNT];
} attribsOut;

uniform mat4 mvp;

vec4 interpole (vec4 v0, vec4 v1, vec4 v2)
{
    return gl_TessCoord.x * v0 +
           gl_TessCoord.y * v1 +
           gl_TessCoord.z * v2;
}

vec3 interpoleVec3 (vec3 v0, vec3 v1, vec3 v2)
{
    return gl_TessCoord.x * v0 +
           gl_TessCoord.y * v1 +
           gl_TessCoord.z * v2;
}


float interpoleFloat (float a, float b, float c)
{
    return gl_TessCoord.x * a +
           gl_TessCoord.y * b +
           gl_TessCoord.z * c;
}

void main()
{
    // TODO: Effectuer l'interpolation des points avec gl_TessCoord
    vec3 p = interpoleVec3 (
        attribsIn[0].worldPos, 
        attribsIn[1].worldPos,
        attribsIn[2].worldPos
    );

    attribsOut.worldPos = p;

    attribsOut.pointSize = interpoleFloat (
        attribsIn[0].pointSize,
        attribsIn[1].pointSize,
        attribsIn[2].pointSize
    );


    for (int i = 0; i < CLIP_COUNT; ++i) {
        attribsOut.clipDist[i] = interpoleFloat(
            attribsIn[0].clipDist[i],
            attribsIn[1].clipDist[i],
            attribsIn[2].clipDist[i]
        );
    }

    gl_Position = mvp * vec4(attribsOut.worldPos, 1.0);
}
