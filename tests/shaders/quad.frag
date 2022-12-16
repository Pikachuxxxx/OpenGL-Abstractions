#version 330 core

in VS_OUT {
    vec2 texCoords;
} vs_in;

uniform sampler2D scene_tex;

out vec4 outFragColor;

void main()
{
    outFragColor = vec4(texture(scene_tex, vs_in.texCoords).rgb, 1.0f);
}
