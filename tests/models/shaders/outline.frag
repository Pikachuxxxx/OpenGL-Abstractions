#version 330 core


in VS_OUT {
    vec3 normal;
    vec2 texCoords;
} vs_in;

out vec4 color;

void main()
{
    // color = vec4(1, 0, 1, 1); // Testing if anythign being rendered
    // color = vec4(vs_in.texCoords, 0.0f, 1.0f); // Testing UV Coordinates
    // color = vec4(vs_in.normal, 1.0f); // Testing Normal cooridinates
    // color = texture(texture_diffuse1, vs_in.texCoords);
    color = vec4(1.0f, 0.63f, 0.0f, 1.0f);
}
