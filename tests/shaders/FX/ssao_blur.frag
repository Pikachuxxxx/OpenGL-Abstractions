#version 410 core

#define MAX_SAMPLES 512

in VS_OUT {
    vec2 texCoords;
} vs_in;

uniform sampler2D ssaoInput;

layout (location = 0) out vec4 outFragColor;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, vs_in.texCoords + offset).r;
        }
    }
    float FragColor = result / (4.0 * 4.0);
    outFragColor = vec4(FragColor, FragColor, FragColor, 1.0f);
}
