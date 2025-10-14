#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 texCoords;

#define MAX_SPOT_LIGHTS 16
#define MAX_POINT_LIGHTS 4

out ATTRIBS_VS_OUT
{
    vec2 texCoords;
    vec3 normal;
    vec3 color;
} attribsOut;

out LIGHTS_VS_OUT
{
    vec4 obsPos;
    vec4 dirLightDir;
    
    vec4 spotLightsDir[MAX_SPOT_LIGHTS];
    vec4 spotLightsSpotDir[MAX_SPOT_LIGHTS];
    
    //vec3 pointLightsDir[MAX_POINT_LIGHTS];
} lightsOut;

uniform mat4 mvp;
uniform mat4 view;
uniform mat4 modelView;
uniform mat3 normalMatrix;

struct Material
{
    vec4 emission;
    vec4 ambient;
    vec4 diffuse;
    vec3 specular;
    float shininess;
};

struct DirectionalLight
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;  
    vec4 direction;
};

struct SpotLight
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    
    vec4 position;
    vec3 direction;
    float exponent;
    float openingAngle;
};

uniform int nSpotLights;
uniform vec4 globalAmbient;

layout (std140) uniform MaterialBlock
{
    Material mat;
};

layout (std140) uniform LightingBlock
{
    DirectionalLight dirLight;
    SpotLight spotLights[MAX_SPOT_LIGHTS];
};

void main()
{
    float dummy = mat.shininess;
    // Attribs
    gl_Position = mvp * vec4(position, 1.0);
    attribsOut.texCoords = texCoords;
    attribsOut.color = color;

    vec3 transformedNormal = normalMatrix * normal;

    // TODO: Écriture des attributs de sortie
    //       Si la normale est nul, lui donner une valeur qui pointe vers le haut.
    if (length(normal) < 0.0001) {
        attribsOut.normal = vec3(0.0, 1.0, 0.0); 
    } else {
        attribsOut.normal = normalize(transformedNormal);
    }

    // Lights
    lightsOut.obsPos = (modelView * vec4(position, 1.0)); 
    lightsOut.dirLightDir = (view * vec4(dirLight.direction.xyz, 0.0));
    // TODO: Écriture des propriétés de lumières en sortie    
    for(int i = 0; i < nSpotLights; i++)
    {
        vec3 spotPosVS = vec3(view * (spotLights[i].position));
        lightsOut.spotLightsDir[i] = vec4(normalize(spotPosVS - lightsOut.obsPos.xyz),1.0);
        lightsOut.spotLightsSpotDir[i] = normalize(view * vec4(spotLights[i].direction, 0.0));
    }
    
}
