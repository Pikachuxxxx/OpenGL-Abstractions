#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 biTangent;


out VS_OUT {
    vec3 normal;
    vec2 texCoords;
    vec3 tangent;
    vec3 biTangent;
    mat3 TBN;
    vec3 worldPos;
    vec4 FragPos;
} vs_out;

uniform mat4 u_Model = mat4(1.0f);

layout (std140) uniform VPMatrices
{
    mat4 u_View;
    mat4 u_Projection;
};

void main()
{
    gl_PointSize = 50.0;
    gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0f);
    mat3 normalMatrix = transpose(inverse(mat3(u_View * u_Model)));
    vs_out.normal = normalMatrix * normal;//normalize(vec3(u_Projection * vec4(normalMatrix * normal, 1.0)));
    vs_out.texCoords = texCoords;
    vs_out.tangent = tangent;
    vs_out.biTangent = biTangent;

    vs_out.FragPos = u_Projection * u_View * u_Model * vec4(position, 1.0f);
    vs_out.worldPos = (u_Model * vec4(position, 1.0f)).xyz;
}
