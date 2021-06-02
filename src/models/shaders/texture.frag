#version 330 core

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
} vs_in;

out vec4 color;

uniform vec3 lightColor;
uniform bool enableDepthColors;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_ambient;

void main()
{
    // color = vec4(1, 0, 1, 1); // Testing if anythign being rendered
    // color = vec4(vs_in.texCoords, 0.0f, 1.0f); // Testing UV Coordinates
    // color = vec4(vs_in.normal, 1.0f); // Testing Normal cooridinates
    // color = vec4(lightColor, 1.0f);
    if(!enableDepthColors)
        color = texture(texture_diffuse, vs_in.texCoords);
    else
        color = vec4(vec3(gl_FragCoord.z), 1.0f);
}
