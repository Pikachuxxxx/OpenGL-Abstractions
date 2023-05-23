#version 410 core

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
    vec3 tangent;
    vec3 biTangent;
    mat3 TBN;
    vec3 worldPos;
    vec4 FragPos;
} vs_in;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metalRoughnessMap;

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

void main() {
    // store the fragment position vector in the first gbuffer texture
    gPosition = vec4(vs_in.FragPos.xyz, 1.0f);
    // gPosition = vs_in.FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = vec4(normalize(vs_in.normal), 1.0f);
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(albedoMap, vs_in.texCoords).rgb;
    // store specular intensity in gAlbedoSpec’s alpha component
    // metalRoughnessMap : R => AO, G channel => roughness values, B => metallic values
    gAlbedoSpec.a = 1.0f;//texture(metalRoughnessMap, vs_in.texCoords).r;
}
