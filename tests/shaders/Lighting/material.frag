#version 330 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
    vec3 FragPos;
    vec3 lightPos;
} vs_in;

out vec4 color;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform Material material;

uniform sampler2D texture_diffuse1;

void main()
{
    // Ambient lighting
    vec3 ambient = vec3(0.1) * lightColor * material.ambient;

    // Diffuse Lighting
    vec3 normal = normalize(vs_in.normal);
    vec3 lightDir = normalize(vs_in.lightPos - vs_in.FragPos);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = lightColor * (diff * material.diffuse);

    // Specular shading
    vec3 viewDir = normalize(-vs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = lightColor * (spec * material.specular);

    // Calculate the final lighting color
    vec3 result = (ambient + diffuse + specular);
    color = vec4(result, 1.0f);
}
