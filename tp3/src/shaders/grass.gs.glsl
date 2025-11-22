#version 330 core

// TODO: À remplir
layout(points) in;
layout(triangle_strip, max_vertices = 7) out;

//out ATTRIBS_TES_OUT // Sent from tess
//{
//vec3 worldPos;
//float pointSize;
//float clipDist[];
//} attribsOut[];
//const int CLIP_COUNT = 2;

in ATTRIBS_TES_OUT
{
    // TODO: Ajouter les attributs si nécessaire
    vec3 worldPos;
    float height;
} attribsIn[];

out ATTRIBS_GS_OUT
{
    // TODO: Ajouter les attributs si nécessaire
    float height;
} attribsOut;


uniform mat4 mvp;
uniform mat4 modelView;

// Fonction pseudo aléatoire, utiliser le paramètre co pour avoir une valeur différente en sortie
float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    vec3 base = attribsIn[0].worldPos;

    float h = 0.3 + rand(base.xy) * 0.5;
    float angleY = rand(base.xz) * 6.2831853;
    float bendX = (rand(base.yz) - 0.5) * 0.4;
    float width = 0.02 + rand(base.yx) * 0.02;
    
    vec3 right = vec3(cos(angleY), 0.0, sin(angleY)) * width;
    vec3 forward = vec3(-sin(angleY), 0.0, cos(angleY));

    int segments = 3;
    for (int i = 0; i <= segments; i++)
    {
        float t = float(i) / float(segments);
        float currentWidth = width * (1.0 - t * 0.7);

        float bend = t * t * bendX;
        vec3 offset = vec3(0.0, t * h, 0.0) + forward * bend;
        
        vec3 pos = base + offset;
        vec3 r = right * (1.0 - t * 0.7);

        attribsOut.height = t;
        gl_Position = mvp * vec4(pos - r, 1.0);
        EmitVertex();
        
        attribsOut.height = t;
        gl_Position = mvp * vec4(pos + r, 1.0);
        EmitVertex();
    }
    
    EndPrimitive();
}
