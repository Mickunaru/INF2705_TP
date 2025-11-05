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
    
    vec3 Ln = normalize(spotDir);
    vec3 L  = normalize(-lightDir);
    
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
    vec3 N = normalize(attribsIn.normal);
    vec3 V = normalize(lightsIn.obsPos);

    vec3 color = mat.emission + globalAmbient * mat.ambient;

    const float LEVELS = 4;

    color += dirLight.ambient * mat.ambient;

    vec3 L = normalize(lightsIn.dirLightDir);
    float diffuseIntensity = max(dot(N, L), 0.0);

    diffuseIntensity = floor(diffuseIntensity * LEVELS) / LEVELS;
    color += dirLight.diffuse * mat.diffuse * diffuseIntensity;

    if(diffuseIntensity > 0.0)
    {
        vec3 R = reflect(-L, N);
        float specularIntensity = max(dot(R, V), 0.0);
        specularIntensity = pow(specularIntensity, mat.shininess);

        specularIntensity = floor(specularIntensity * LEVELS) / LEVELS;
        color += dirLight.specular * mat.specular * specularIntensity;
    }
    
    for(int i = 0; i < nSpotLights; i++)
    {
        color += spotLights[i].ambient * mat.ambient;
        
        float spotFactor = computeSpot(
            spotLights[i].openingAngle,
            spotLights[i].exponent,
            lightsIn.spotLightsSpotDir[i],
            lightsIn.spotLightsDir[i],
            N
        );
        
        if(spotFactor > 0.0)
        {
            float distance = length(lightsIn.spotLightsDir[i]);
            float attenuation = 1.0 - smoothstep(7.0, 10.0, distance);
            
            float combinedFactor = spotFactor * attenuation;
            
            vec3 L_spot = normalize(lightsIn.spotLightsDir[i]);
            float diffuse = max(dot(N, L_spot), 0.0);
            color += spotLights[i].diffuse * mat.diffuse * diffuse * combinedFactor;
            
            if(diffuse > 0.0)
            {
                vec3 R_spot = reflect(-L_spot, N);
                float specular = max(dot(R_spot, V), 0.0);
                specular = pow(specular, mat.shininess);
                color += spotLights[i].specular * mat.specular * specular * combinedFactor;
            }
        }
    }

    vec4 texColor = texture(diffuseSampler, attribsIn.texCoords);
    vec3 finalColor = (attribsIn.color == vec3(0.0)) ? texColor.rgb : attribsIn.color;
    FragColor = vec4(finalColor * color, texColor.a);
}
