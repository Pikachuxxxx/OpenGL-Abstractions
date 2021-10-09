#version 330 core

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
} vs_in;

out vec4 color;

uniform vec3 viewPos;
uniform Light light;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

void main()
{
    vec3 FragPos = texture(gPosition, vs_in.texCoords).rgb;
    vec3 Normal = texture(gNormal, vs_in.texCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, vs_in.texCoords).rgb;
    float Specular = texture(gAlbedoSpec, vs_in.texCoords).a;

    // Ambient lighting
    vec3 ambient = light.ambient * Albedo;

    // Diffuse Lighting
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * (diff * Albedo);

    // Specular shading
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 16.0);
    // vec3 specular = light.specular * (spec * vec3(texture(material.specular, vs_in.texCoords)));
    vec3 specular = light.specular * (spec * Specular);

    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // Calculate the final lighting color
    vec3 result = (ambient + diffuse + specular);
    color = vec4(result, 1.0f);
    // color = vec4(vec3(texture(material.emission, vs_in.texCoords)), 1.0f);

    // Gamma correction
    // float gamma = 2.2;
    // vec3 fragcolor = pow(result, vec3(1.0/gamma));
    // color = vec4(Normal, 1.0f);
}
