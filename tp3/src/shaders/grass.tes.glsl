#version 400 core

layout(triangles, equal_spacing, ccw, point_mode) in;

in ATTRIBS_TES_IN {
    vec3 worldPos;
} attribsIn[];

out ATTRIBS_TES_OUT 
{
    vec3 worldPos;
    float height;
} attribsOut;

uniform mat4 mvp;

vec3 interpoleVec3(vec3 v0, vec3 v1, vec3 v2)
{
    return gl_TessCoord.x * v0 +
           gl_TessCoord.y * v1 +
           gl_TessCoord.z * v2;
}
void main()
{
    vec3 p = interpoleVec3 (
        attribsIn[0].worldPos, 
        attribsIn[1].worldPos,
        attribsIn[2].worldPos
    );

    attribsOut.worldPos = p;
    attribsOut.height = 0.5;

    gl_Position = mvp * vec4(attribsOut.worldPos, 1.0);
}
