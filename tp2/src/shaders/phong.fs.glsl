#version 330 core

#define MAX_SPOT_LIGHTS 8
#define MAX_POINT_LIGHTS 4

in ATTRIBS_VS_OUT // Inputs calculée dans vs
{
    vec2 texCoords;
    vec3 normal;
    vec3 color;
} attribsIn;

in LIGHTS_VS_OUT // Calculer la contribution de chaque lumière    
{
    vec3 obsPos;
    vec3 dirLightDir;
    
    vec3 spotLightsDir[MAX_SPOT_LIGHTS];
    vec3 spotLightsSpotDir[MAX_SPOT_LIGHTS];
} lightsIn;


struct Material // Comment un obj réagit à la lumière
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

uniform int nSpotLights; // Spotlights actifs

uniform vec3 globalAmbient; 

layout (std140) uniform MaterialBlock // Matériau courant de l'objet
{
    Material mat;
};

layout (std140) uniform LightingBlock // Lumière directionnelle et spotlights 
{
    DirectionalLight dirLight;
    SpotLight spotLights[MAX_SPOT_LIGHTS];
};

uniform sampler2D diffuseSampler;

in vec3 fragPos;
out vec4 FragColor;


float computeSpot(in float openingAngle, in float exponent, in vec3 spotDir, in vec3 lightDir)
{
// TODO: Calcul de spotlight, l'algorithme classique d'OpenGL vu en classe (voir annexe).
    float cosGamma = dot(normalize(-spotDir), normalize(lightDir));
    float cosDelta = cos(openingAngle);

    if(cosGamma > cosDelta)
    {
        return pow(cosGamma, exponent);
    }
    return 0.0;
}

void main()
{
    // TODO: Calcul d'illumination
    vec3 N = normalize(attribsIn.normal);
    vec3 V = normalize(-lightsIn.obsPos);
    vec3 texColor = texture(diffuseSampler, attribsIn.texCoords).rgb; // Crgb
    
    
    // Ambiente
    vec3 ambient = globalAmbient * mat.ambient * texColor;
    
    // TODO: Seulement la lumière directionnel à l'effet de cel-shading, sur la composante diffuse et spéculaire
    vec3 L= normalize(-dirLight.direction);
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse_dir = diff * dirLight.diffuse * texColor;

    vec3 R = reflect( -L, N);
    float spec = pow(max(dot(R, V), 0.0), mat.shininess);
    vec3 specular_dir = spec * dirLight.specular * mat.specular;
    specular_dir = vec3(0);
    //const float LEVELS = 4;

    // Spot light
    vec3 spotResults = vec3(0.0);
    for(int i = 0; i < nSpotLights; i++)
    {
        // TODO: Calcul des spotlights
    
        // Utiliser un facteur d'atténuation. On peut utiliser smoothstep avec la distance
        // entre la surface illuminé et la source de lumière. Il devrait y avoir un effet de blending
        // entre 7 et 10 unitées.
        vec3 L_spot = normalize(lightsIn.spotLightsDir[i]);
        float diff_spot = max(dot(N, L_spot), 0.0);
        float spotFactor = computeSpot(spotLights[i].openingAngle,
                                        spotLights[i].exponent,
                                        lightsIn.spotLightsDir[i],
                                        L_spot);
        vec3 R_s= reflect(-L_spot, N);
        float spec_spot= pow(max(dot(R_s, V), 0.0), mat.shininess);
        spotResults += spotFactor * (diff_spot * spotLights[i].diffuse * texColor + 
                                    spec_spot  + spotLights[i].specular * mat.specular);
    }

    vec3 finalColor = ambient + diffuse_dir + specular_dir + spotResults + mat.emission;
    FragColor = vec4(finalColor, 1.0);
}