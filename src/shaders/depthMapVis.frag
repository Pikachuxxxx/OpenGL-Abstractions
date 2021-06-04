#version 330 core

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
} vs_in;

out vec4 color;

uniform sampler2D depthTexture;

void main()
{
    float depthValue = texture(depthTexture, vs_in.texCoords).r;
    color = vec4(vec3(depthValue), 1.0f);
    // color = vec4(1, 0, 1, 1);
}
