#version 400 core

// TODO: À remplir
layout(vertices = 3) out;

uniform float tessLevel = 8.0;
uniform mat4 modelView;
uniform mat4 mvp;

in ATTRIBS_VS_OUT {
    vec3 worldPos;
} attribsIn[];

out ATTRIBS_TCS_OUT {
    vec3 worldPos;
} attribsOut[];


float tessLevelFromDistance(float dist)
{
    const float MIN_TESS = 2.0;
    const float MAX_TESS = 32.0;
    const float MIN_DIST = 10.0;
    const float MAX_DIST = 40.0;
    float t = clamp((dist - MIN_DIST) / (MAX_DIST - MIN_DIST), 0.0, 1.0);
    return mix(MAX_TESS, MIN_TESS, t);
}

void main()
{
    // Chaque TCS recopie attributs entrée -> sortie
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    attribsOut[gl_InvocationID].worldPos = attribsIn[gl_InvocationID].worldPos;

    const float MIN_TESS = 2.0;
    const float MAX_TESS = 32.0;

    const float MIN_DIST = 10.0;
    const float MAX_DIST = 40.0;
    
    // TODO: Définir le niveau de tessellation une fois par patch.
    if (gl_InvocationID == 0)
        {
//            // Transforme les points dans l'espace caméra
//            vec3 p0 = (mvp * gl_in[0].gl_Position).xyz;
//            vec3 p1 = (mvp * gl_in[1].gl_Position).xyz;
//            vec3 p2 = (mvp * gl_in[2].gl_Position).xyz; 
//
//            // Utiliser la distance du point milieu de l'arête traité par rapport à la caméra
//            // comme facteur pour déterminer le niveau de tessellation.
//            vec3 center = (p0 + p1 + p2) / 3.0;
//            float distCamera = length(center);
//
//            // Une fonction par palié donnera un meilleur résultat pour déterminer le niveau de tessellation
//            // selon la distance.
//            float tess = tessLevelFromDistance(distCamera);

            vec3 center = (attribsIn[0].worldPos + attribsIn[1].worldPos + attribsIn[2].worldPos) / 3.0;
            vec3 viewPos = (modelView * vec4(center, 1.0)).xyz;
            float dist = length(viewPos);
            float tess = tessLevelFromDistance(dist);
        
            // La tessellation extérieur
            gl_TessLevelOuter[0] = tessLevel;
            gl_TessLevelOuter[1] = tessLevel;
            gl_TessLevelOuter[2] = tessLevel;
            // La tessellation de l'interieur est le maximum des tessellations extérieurs
            gl_TessLevelInner[0] = tessLevel;

        }
}
