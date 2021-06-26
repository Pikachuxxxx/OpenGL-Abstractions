#version 330 core

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
    vec3 lightColor;
} vs_in;

out vec4 color;

uniform vec3 objectColor;

void main()
{
    color = vec4(vs_in.lightColor * objectColor, 1.0f);
}
