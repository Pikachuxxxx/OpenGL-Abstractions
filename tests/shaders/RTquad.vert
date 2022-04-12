#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT {
    vec3 normal;
    vec2 texCoords;
} vs_out;

void main()
{
    gl_Position = vec4(position, 1.0f);
    vs_out.normal = normal;
    vs_out.texCoords = texCoords;
}
