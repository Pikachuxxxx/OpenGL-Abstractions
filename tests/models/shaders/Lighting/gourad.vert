#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT {
    vec3 normal;
    vec2 texCoords;
    vec3 lightColor;
} vs_out;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform mat4 u_Model = mat4(1.0f);
layout (std140) uniform VPMatrices
{
    mat4 u_View;
    mat4 u_Projection;
};

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0f);
    vs_out.normal = mat3(transpose(inverse(u_Model))) * normal;
    vs_out.texCoords = texCoords;

    vec3 Position = vec3(u_Model * vec4(position, 1.0));

    // Ambient lighting
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(vs_out.normal);

    // Diffuse Lighting
    vec3 lightDir = normalize(lightPos - Position);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    // Specular shading
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - Position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Calculate the final lighting color
    vec3 result = (ambient + diffuse + specular);
    vs_out.lightColor = result;
}
