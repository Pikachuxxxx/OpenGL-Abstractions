#version 330 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 direction;
    float cutOff;

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
uniform vec3 objectColor;
uniform Material material;
uniform Light light;

void main()
{
    vec3 lightDir = normalize(light.position - vs_in.FragPos);

    // Ambient lighting
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, vs_in.texCoords));
    // check if lighting is inside the spotlight cone
    float theta = dot(lightDir, normalize(-light.direction));

    if(theta > light.cutOff)
    {
        // Diffuse Lighting
        vec3 normal = normalize(vs_in.normal);
        float diff = max(dot(normal, lightDir), 0.0f);
        vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, vs_in.texCoords)));

        // Specular shading
        vec3 viewDir = normalize(viewPos - vs_in.FragPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
        vec3 specular = light.specular * (spec * vec3(texture(material.specular, vs_in.texCoords)));

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
    }
    else
        color = vec4(light.ambient * vec3(texture(material.diffuse, vs_in.texCoords)), 1.0f);

}
