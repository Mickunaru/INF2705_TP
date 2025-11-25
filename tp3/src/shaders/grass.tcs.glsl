#version 400 core

layout(vertices = 3) out;

uniform mat4 modelView;

in ATTRIBS_VS_OUT {
    vec3 worldPos;
} attribsIn[];

out ATTRIBS_TCS_OUT {
    vec3 worldPos;
} attribsOut[];


float tessLevelFromDistance(float dist)
{
    const float MIN_TESS = 2;
    const float MAX_TESS = 32;

    const float MIN_DIST = 10.0f;
    const float MAX_DIST = 40.0f;

    float t = smoothstep(MIN_DIST, MAX_DIST, dist);
    return mix(MAX_TESS, MIN_TESS, t);
}

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    attribsOut[gl_InvocationID].worldPos = attribsIn[gl_InvocationID].worldPos;
    
    if (gl_InvocationID == 0)
        {
            vec3 p0 = (modelView * gl_in[0].gl_Position).xyz;
            vec3 p1 = (modelView * gl_in[1].gl_Position).xyz;
            vec3 p2 = (modelView * gl_in[2].gl_Position).xyz;

            vec3 m01 = (p0 + p1) * 0.5;
            vec3 m12 = (p1 + p2) * 0.5;
            vec3 m20 = (p2 + p0) * 0.5;

            float d01 = length(m01);
            float d12 = length(m12);
            float d20 = length(m20);

            float t01 = tessLevelFromDistance(d01);
            float t12 = tessLevelFromDistance(d12);
            float t20 = tessLevelFromDistance(d20);

            gl_TessLevelOuter[0] = t12;
            gl_TessLevelOuter[1] = t20;
            gl_TessLevelOuter[2] = t01;

            gl_TessLevelInner[0] = max(t12, max(t20, t01));
        }
}
