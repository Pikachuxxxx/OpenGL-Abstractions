#version 330 core

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
    vec3 FragPos;
    vec3 lightPos;
} vs_in;

out vec4 color;

uniform vec3 objectColor;
uniform vec3 lightColor;

uniform sampler2D texture_diffuse1;

void main()
{
    // Ambient lighting
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse Lighting
    vec3 normal = normalize(vs_in.normal);
    vec3 lightDir = normalize(vs_in.lightPos - vs_in.FragPos);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    // Specular shading
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(-vs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Calculate the final lighting color
    vec3 result = (ambient + diffuse + specular) * objectColor;
    color = vec4(result, 1.0f);
}
