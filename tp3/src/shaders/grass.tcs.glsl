#version 400 core

// TODO: À remplir
layout(vertices = 3) out;

uniform mat4 modelView;

float tessLevelFromDistance(float dist)
{
    const float MIN_TESS = 2.0;
    const float MAX_TESS = 32.0;

    const float MIN_DIST = 10.0;
    const float MAX_DIST = 40.0;

    // Clamp la distance dans l’intervalle utile
    float t = clamp((dist - MIN_DIST) / (MAX_DIST - MIN_DIST), 0.0, 1.0);

    // Interpolation inverse : plus proche ⇒ tessel élevé
    return mix(MAX_TESS, MIN_TESS, t);
}

void main()
{
    // Chaque TCS recopie attributs entrée -> sortie
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    
    // TODO: Définir le niveau de tessellation une fois par patch.
    if (gl_InvocationID == 0)
        {
            // Transforme les points dans l'espace caméra
            vec3 p0 = (modelView * gl_in[0].gl_Position).xyz;
            vec3 p1 = (modelView * gl_in[1].gl_Position).xyz;
            vec3 p2 = (modelView * gl_in[2].gl_Position).xyz; 

            // Utiliser la distance du point milieu de l'arête traité par rapport à la caméra
            // comme facteur pour déterminer le niveau de tessellation.
            vec3 center = (p0 + p1 + p2) / 3.0;
            float distCamera = length(center);

            // Une fonction par palié donnera un meilleur résultat pour déterminer le niveau de tessellation
            // selon la distance.
            float tess = tessLevelFromDistance(distCamera);
        
            // La tessellation extérieur
            gl_TessLevelOuter[0] = tess;
            gl_TessLevelOuter[1] = tess;
            gl_TessLevelOuter[2] = tess;

            // La tessellation de l'interieur est le maximum des tessellations extérieurs
            gl_TessLevelInner[0] = tess;

        }
}
