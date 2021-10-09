#version 330 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
    vec4 FragPos;
} vs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main() {
    // store the fragment position vector in the first gbuffer texture
    gPosition = vec4(vs_in.FragPos.xyz, 1.0f);
    // gPosition = vs_in.FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = vec4(normalize(vs_in.normal), 1.0f);
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(texture_diffuse1, vs_in.texCoords).rgb;
    // store specular intensity in gAlbedoSpec’s alpha component
    gAlbedoSpec.a = texture(texture_specular1, vs_in.texCoords).r;
}
