#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 biTangent;

uniform mat4 u_LightSpaceMatrix;
uniform mat4 u_Model;

void main()
{
    gl_Position = u_LightSpaceMatrix * u_Model * vec4(position, 1.0f);
}
