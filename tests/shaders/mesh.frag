#version 330 core

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
} vs_in;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
    color = vec4(vs_in.normal, 1.0f); // Testing Normal cooridinates
    color = vec4(vs_in.texCoords, 0.0f, 1.0f); // Testing UV Coordinates
    color = vec4(texture(texture_diffuse1, vs_in.texCoords).xy, 0.0f, 1.0f);
    color = texture(texture_specular1, vs_in.texCoords);
    // color = vec4(lightColor, 1.0f);
    // color = vec4(1, 1, 1, 1);
}
