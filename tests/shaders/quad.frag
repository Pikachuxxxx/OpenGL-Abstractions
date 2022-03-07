#version 330 core

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
} vs_in;

out vec4 outFragColor;

uniform sampler2D tex;

void main()
{
    outFragColor = vec4(vs_in.normal, 1.0f); // Testing Normal cooridinates
    outFragColor = vec4(vs_in.texCoords, 0.0f, 1.0f); // Testing UV cooridinates
    outFragColor = texture(tex, vs_in.texCoords);
}
