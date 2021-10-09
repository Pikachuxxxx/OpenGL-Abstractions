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
    vec4 FragPos;
} vs_in;

out vec4 color;

uniform vec3 lightColor;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
    // color = vec4(vs_in.texCoords, 0.0f, 1.0f); // Testing UV Coordinates
    // color = vec4(vs_in.normal, 1.0f); // Testing Normal cooridinates
    color = texture(texture_diffuse1, vs_in.texCoords);
    // color = texture(texture_specular1, vs_in.texCoords);
    // color = vec4(lightColor, 1.0f);
    // color = vec4(1, 1, 1, 1);
}
