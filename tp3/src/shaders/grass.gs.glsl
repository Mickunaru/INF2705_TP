#version 330 core

// TODO: À remplir
layout(points) in;
layout(triangle_strip, max_vertices = 3) out;

//out ATTRIBS_TES_OUT // Sent from tess
//{
//vec3 worldPos;
//float pointSize;
//float clipDist[];
//} attribsOut[];
const int CLIP_COUNT = 2;

in ATTRIBS_TES_OUT
{
    // TODO: Ajouter les attributs si nécessaire
    vec3 worldPos;
    float height;
    float pointSize;
    float clipDist[CLIP_COUNT];
} attribsIn[];

out ATTRIBS_GS_OUT
{
    // TODO: Ajouter les attributs si nécessaire
    vec3 worldPos;
    float height;
    float pointSize;
    float clipDist[CLIP_COUNT];
} attribsOut;


uniform mat4 mvp;


// Fonction pseudo aléatoire, utiliser le paramètre co pour avoir une valeur différente en sortie
float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}


void copyClipDistances()   
{
    for (int i = 0; i < CLIP_COUNT; ++i){
        attribsOut.clipDist[i] = attribsIn[0].clipDist[i];
    }
}
void main()
{

    vec3 base = attribsIn[0].worldPos;
    float baseHeight= attribsIn[0].height;

    // Random height
    float height = baseHeight + rand(base.xy) * baseHeight;

    // Random rotations
    float angleY = rand(base.xz) * 6.2831853; 
    float angleX = rand(base.xy) * 0.3;      

    mat3 rotY = mat3(
        cos(angleY), 0, sin(angleY),
        0,           1, 0,
        -sin(angleY),0, cos(angleY)
    );

    mat3 rotX = mat3(
        1, 0,          0,
        0, cos(angleX),-sin(angleX),
        0, sin(angleX), cos(angleX)
    );

    mat3 R = rotY * rotX;

    // Triangle vertices
    vec3 p0 = base + R * vec3(-height, 0.0, 0.0);
    vec3 p1 = base + R * vec3(+height, 0.0, 0.0);
    vec3 p2 = base + R * vec3(0.0, height, 0.0); 

    // Emit vertex 0
    attribsOut.worldPos = p0;
    attribsOut.height   = 0.0;
    attribsOut.pointSize = attribsIn[0].pointSize;
    copyClipDistances();
    gl_Position = mvp * vec4(p0, 1.0);
    EmitVertex();

    // Emit vertex 1
    attribsOut.worldPos = p1;
    attribsOut.height   = 0.0;
    attribsOut.pointSize = attribsIn[0].pointSize;
    copyClipDistances();
    gl_Position = mvp * vec4(p1, 1.0);
    EmitVertex();

    // Emit vertex 2
    attribsOut.worldPos = p2;
    attribsOut.height   = 1.0;
    attribsOut.pointSize = attribsIn[0].pointSize;
    copyClipDistances();
    gl_Position = mvp * vec4(p2, 1.0);
    EmitVertex();

    EndPrimitive();
}
