#version 330 core

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
} vs_in;

out vec4 color;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform sampler2D texture_diffuse1;

void main()
{
    color = vec4(lightColor * objectColor, 1.0f);
}
