#version 330 core

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
} vs_in;

out vec4 color;

uniform bool enableDepthColors;
uniform vec3 lightColor;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;

float near = 0.25;
float far = 100.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    // color = vec4(1, 0, 1, 1); // Testing if anythign being rendered
    // color = vec4(vs_in.normal, 1.0f); // Testing Normal cooridinates
    // color = vec4(vs_in.texCoords, 0.0f, 1.0f); // Testing UV Coordinates
    // color = vec4(lightColor, 1.0f);
    if(!enableDepthColors)
    {
        vec4 texColor = texture(texture_diffuse, vs_in.texCoords);
        if(texColor.a < 0.1f)
        discard;
        color = texColor;
    }
    else
    {
        float depth = LinearizeDepth(gl_FragCoord.z) / far;
        color = vec4(vec3(depth), 1.0f);
    }
}
