#version 330 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
} vs_in;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

void main()
{
    color = texture(texture_diffuse1, vs_in.texCoords);
    // color = vec4(1, 0, 1, 1);
}
