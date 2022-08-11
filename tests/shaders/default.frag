#version 410 core

in vec2 inTexCoords;

out vec4 FragColor;

void main()
{
    FragColor = vec4(inTexCoords, 0.0, 1.0);
}
