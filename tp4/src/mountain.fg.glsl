#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;


struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec3 specular;
    float shininess;
};


struct DirectionalLight {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 direction;
};


uniform sampler2D texture1; 
uniform vec3 viewPos;   


layout (std140) uniform MaterialBlock {
    Material material;
};

layout (std140) uniform LightBlock {
    DirectionalLight dirLight;
};

void main()
{
    vec4 texColor = texture(texture1, TexCoords);
    
    vec3 ambient = vec3(dirLight.ambient) * vec3(material.ambient) * vec3(texColor);

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-vec3(dirLight.direction)); 

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(dirLight.diffuse) * (diff * vec3(material.diffuse) * vec3(texColor));

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = vec3(dirLight.specular) * (spec * material.specular);

    vec3 result = ambient + diffuse + specular;
    
    FragColor = vec4(result, 1.0);
}