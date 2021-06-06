#version 330 core

struct Material
{
    sampler2D diffuse;
    // sampler2D specular;
    vec3 specular;
    sampler2D emission;
    sampler2D normalMap;
    float shininess;
};

struct Light
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
    vec3 FragPos;
} vs_in;

out vec4 color;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    // Ambient lighting
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, vs_in.texCoords));

    // Diffuse Lighting
    vec3 normal = texture(material.normalMap, vs_in.texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0); //normalize(vs_in.normal);
    vec3 lightDir = normalize(light.position - vs_in.FragPos);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, vs_in.texCoords)));

    // Specular shading
    vec3 viewDir = normalize(viewPos - vs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    // vec3 specular = light.specular * (spec * vec3(texture(material.specular, vs_in.texCoords)));
    vec3 specular = light.specular * (spec * material.specular);

    // Emission shading
    vec3 emission = vec3(texture(material.emission, vs_in.texCoords));

    // attenuation
    float distance = length(light.position - vs_in.FragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // Calculate the final lighting color
    vec3 result = (ambient + diffuse + specular);
    color = vec4(result, 1.0f);
    // color = vec4(vec3(texture(material.normalMap, vs_in.texCoords)), 1.0f);

    // Gamma correction
    // float gamma = 2.2;
    // vec3 fragcolor = pow(result, vec3(1.0/gamma));
    // color = vec4(fragcolor, 1.0f);
}
