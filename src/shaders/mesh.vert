#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT {
    vec3 normal;
    vec2 texCoords;
    vec3 FragPos;
} vs_out;

uniform vec3 lightPos;

uniform mat4 u_Model = mat4(1.0f);
layout (std140) uniform VPMatrices
{
    mat4 u_View;
    mat4 u_Projection;
};

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0f);
    // mat3 normalMatrix = mat3(transpose(inverse(u_View * u_Model)));
    // vs_out.normal = normalize(vec3(u_Projection * vec4(normalMatrix * normal, 1.0)));
    vs_out.normal = mat3(transpose(inverse(u_Model))) * normal;
    vs_out.texCoords = texCoords;
    vs_out.FragPos = vec3(u_Model * vec4(position, 1.0f));
}
