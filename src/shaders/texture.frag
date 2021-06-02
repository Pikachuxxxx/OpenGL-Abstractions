#version 330 core

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
} vs_in;

out vec4 color;

uniform vec3 lightColor;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

void main()
{
    // color = vec4(1, 0, 1, 1); // Testing if anythign being rendered
    // color = vec4(vs_in.texCoords, 0.0f, 1.0f); // Testing UV Coordinates
    // color = vec4(vs_in.normal, 1.0f); // Testing Normal cooridinates
    // color = vec4(lightColor, 1.0f);
    color = texture(texture_diffuse, vs_in.texCoords);
}
