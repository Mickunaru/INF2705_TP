#version 330 core

#define MAX_SPOT_LIGHTS 8
#define MAX_POINT_LIGHTS 4

in ATTRIBS_VS_OUT
{
    vec2 texCoords;
    vec3 normal;
    vec3 color;
} attribsIn;

in LIGHTS_VS_OUT
{
    vec3 obsPos;
    vec3 dirLightDir;
    
    vec3 spotLightsDir[MAX_SPOT_LIGHTS];
    vec3 spotLightsSpotDir[MAX_SPOT_LIGHTS];
    
    vec3 pointLightsDir[MAX_POINT_LIGHTS];
} lightsIn;


struct Material
{
    vec3 emission;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirectionalLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    vec3 direction;
};

struct SpotLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    vec3 position;
    vec3 direction;
    float exponent;
    float openingAngle;
};

uniform int nSpotLights;

uniform vec3 globalAmbient;

layout (std140) uniform MaterialBlock
{
    Material mat;
};

layout (std140) uniform LightingBlock
{
    DirectionalLight dirLight;
    SpotLight spotLights[MAX_SPOT_LIGHTS];
};

uniform sampler2D diffuseSampler;

out vec4 FragColor;


float computeSpot(in float openingAngle, in float exponent, in vec3 spotDir, in vec3 lightDir, in vec3 normal)
{
    float spotFactor = 0.0;
    
    // TODO: Calcul de spotlight, l'algorithme classique d'OpenGL vu en classe (voir annexe).
    vec3 Ln = normalize(spotDir);
    vec3 L  = normalize(lightDir);
    
    float cosGamma = dot(L, Ln);
    float cosDelta = cos(openingAngle);

    if(cosGamma > cosDelta)
    {
        spotFactor = pow(cosGamma, exponent);
    }

    return spotFactor;
}

void main()
{
    // TODO: Calcul d'illumination

    // Directional light

    // TODO: Seulement la lumière directionnel à l'effet de cel-shading, sur la composante diffuse et spéculaire
    const float LEVELS = 4;

    // Spot light
    
    for(int i = 0; i < nSpotLights; i++)
    {
        // TODO: Calcul des spotlights
    
        // Utiliser un facteur d'atténuation. On peut utiliser smoothstep avec la distance
        // entre la surface illuminé et la source de lumière. Il devrait y avoir un effet de blending
        // entre 7 et 10 unitées.
        // Le facteur impacte la composante diffuse et spéculaire.
    }

    //color += normal/2.0 + vec3(0.5); // DEBUG: Show normals
    vec4 texColor = texture(diffuseSampler, attribsIn.texCoords);
    vec3 finalColor = (attribsIn.color == vec3(0.0)) ? texColor.rgb : attribsIn.color;
    FragColor = vec4(finalColor, texColor.a);
}
