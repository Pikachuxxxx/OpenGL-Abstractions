#version 330 core

in VS_OUT {
    vec3 texCoords;
} vs_in;

out vec4 color;

uniform samplerCube cubeMap;

void main()
{
    color = vec4(vs_in.texCoords, 1);
    color = texture(cubeMap, vs_in.texCoords);
}
