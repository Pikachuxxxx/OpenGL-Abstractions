#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 2) in vec3 tangent;
layout (location = 2) in vec3 biTangent;

uniform mat4 u_Model;
layout (std140) uniform VPMatrices
{
    mat4 u_View;
    mat4 u_Projection;
};

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0f);
}
