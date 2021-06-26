#version 330 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
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
} vs_in;

out vec4 color;

uniform vec3 viewPos;
uniform vec3 objectColor;

uniform Material material;
uniform Light light;

void main()
{
    // Ambient lighting
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, vs_in.texCoords));

    // Diffuse Lighting
    vec3 normal = normalize(vs_in.normal);
    vec3 lightDir = normalize(light.position - vs_in.FragPos);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, vs_in.texCoords)));

    // Specular shading
    vec3 viewDir = normalize(viewPos - vs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, vs_in.texCoords)));

    // Emission shading
    vec3 emission = vec3(texture(material.emission, vs_in.texCoords));

    // Calculate the final lighting color
    vec3 result = (ambient + diffuse + specular);
    color = vec4(result, 1.0f);
    // color = vec4(vec3(texture(material.specular, vs_in.texCoords)), 1.0f);
}
