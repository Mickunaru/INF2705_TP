#version 330 core

#define MAX_SPOT_LIGHTS 16
#define MAX_POINT_LIGHTS 4

in ATTRIBS_VS_OUT
{
    vec2 texCoords;
    vec3 normal;
    vec3 color;
} attribsIn;

in LIGHTS_VS_OUT
{
    vec4 obsPos;
    vec4 dirLightDir;
    
    vec4 spotLightsDir[MAX_SPOT_LIGHTS];
    vec4 spotLightsSpotDir[MAX_SPOT_LIGHTS];
    
    //vec3 pointLightsDir[MAX_POINT_LIGHTS];
} lightsIn;


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

uniform sampler2D diffuseSampler;

out vec4 FragColor;


float computeSpot(in float openingAngle, in float exponent, in vec3 spotDir, in vec3 lightDir, in vec3 normal)
{
    float spotFactor = 0.0;
    
    // TODO: Calcul de spotlight, l'algorithme classique d'OpenGL vu en classe (voir annexe).
    float cosCutoff = cos(radians(openingAngle));

    float cosAngle = dot(-lightDir, spotDir);
    

    if (cosAngle > cosCutoff)
    {
        spotFactor = pow(max(0.0, cosAngle), exponent);
    }
    
    return spotFactor;
}

void main()
{
    // TODO: Calcul d'illumination
    vec3 N = normalize(attribsIn.normal);
    vec3 V = normalize(-lightsIn.obsPos.xyz);
    vec3 L_dir = normalize(lightsIn.dirLightDir.xyz);
    // Directional light
    
    // TODO: Seulement la lumière directionnel à l'effet de cel-shading, sur la composante diffuse et spéculaire
    const float LEVELS = 4;
       
    
    vec4 textureColor = vec4(texture(diffuseSampler, attribsIn.texCoords).rgb,1.0);
    vec4 finalColor = globalAmbient * mat.ambient;

    float diffIntensity = max(0.0, dot(N, L_dir));
    float specIntensity = 0.0;

    if (diffIntensity > 0.0) {
        vec3 R = reflect(-L_dir, N);
        specIntensity = pow(max(0.0, dot(R, V)), mat.shininess);
    }

    float diffLevel = floor(diffIntensity * LEVELS) / (LEVELS - 1.0);
    float specLevel = floor(specIntensity * LEVELS) / (LEVELS - 1.0);
    
    finalColor += dirLight.ambient * mat.ambient;
    finalColor += dirLight.diffuse * mat.diffuse * textureColor * diffLevel;
    finalColor += dirLight.specular * vec4(mat.specular, 1.0) * specLevel;
    // Spot light
    for(int i = 0; i < nSpotLights; i++)
    {
        // TODO: Calcul des spotlights
        vec3 L_spot = normalize(lightsIn.spotLightsDir[i].xyz);
        float distance = length(spotLights[i].position.xyz - lightsIn.obsPos.xyz);
        float attenuation = smoothstep(10.0, 7.0, distance); 
        vec3 spotDirVS = lightsIn.spotLightsSpotDir[i].xyz;
        float spotFactor = computeSpot(spotLights[i].openingAngle, spotLights[i].exponent, 
                                      spotDirVS, L_spot, N);
        float spotDiffIntensity = max(0.0, dot(N, L_spot));
        float spotDiffLevel = floor(spotDiffIntensity * LEVELS) / (LEVELS - 1.0);

        finalColor += spotLights[i].ambient * mat.ambient * attenuation;
        finalColor += spotLights[i].diffuse* mat.diffuse * vec4(textureColor.rgb, 1.0) * attenuation * spotFactor * spotDiffLevel;
    
        // Utiliser un facteur d'atténuation. On peut utiliser smoothstep avec la distance
        // entre la surface illuminé et la source de lumière. Il devrait y avoir un effet de blending
        // entre 7 et 10 unitées.
        // Le facteur impacte la composante diffuse et spéculaire.
        if (spotDiffIntensity > 0.0) {
            vec3 R_spot = reflect(-L_spot, N);
            float spotSpecIntensity = pow(max(0.0, dot(R_spot, V)), mat.shininess);
            float spotSpecLevel = floor(spotSpecIntensity * LEVELS) / (LEVELS - 1.0);
            
            finalColor += spotLights[i].specular * vec4(mat.specular, 1.0) * attenuation * spotFactor * spotSpecLevel;
        }
    }
    FragColor = finalColor;

    //vec3 color = vec3(0);
    //color += normal/2.0 + vec3(0.5); // DEBUG: Show normals
    
}
