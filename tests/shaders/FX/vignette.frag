#version 330 core

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
} vs_in;

out vec4 color;

uniform sampler2D renderTex;
uniform vec2 resolution;
uniform float intensity;
uniform float extent;

void main()
{
    vec2 uv = vec2(gl_FragCoord.xy / resolution);
    uv *= 1.0 - uv.yx;
    float vig = uv.x * uv.y * intensity; // Intensity
    vig = pow(vig, extent); // extent;
    color = vec4(vec3(vig), 1.0f) * texture(renderTex, vs_in.texCoords);
}
