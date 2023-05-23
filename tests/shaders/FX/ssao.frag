#version 410 core

#define MAX_SAMPLES 512

in VS_OUT {
    vec2 texCoords;
} vs_in;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noiseTexture;
uniform vec3 samples[MAX_SAMPLES];
uniform mat4 u_Projection;
uniform float ssaoRadius;
uniform int kernelSize;
uniform float bias; // TODO: Make this a uniform

const vec2 noiseScale = vec2(1280.0/4.0, 720.0/4.0); // screen = 800x600

layout (location = 0) out vec4 outFragColor;

void main()
{
    vec3 fragPos = texture(gPosition, vs_in.texCoords).rgb;
    vec3 normal = texture(gNormal, vs_in.texCoords).rgb;
    vec3 randomVec = texture(noiseTexture, vs_in.texCoords * noiseScale).rgb;

    vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normal);

    // Next we iterate over each of the kernel samples, transform the samples from tangent to view-space,
    // add them to the current fragment position, and compare the fragment position's depth with the sample
    // depth stored in the view-space position buffer

    float occlusion = 0.0f;
    for (int i = 0; i < kernelSize; ++i) {
        // get sample position
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * ssaoRadius;

        // Next we want to transform sample to screen-space so we can sample the position/depth value of sample
        vec4 offset = vec4(samplePos, 1.0);
        offset      = u_Projection * offset;    // from view to clip-space
        offset.xyz /= offset.w;               // perspective divide
        offset.xyz  = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        float sampleDepth = texture(gPosition, offset.xy).z;

        float rangeCheck = smoothstep(0.0, 1.0, ssaoRadius / abs(fragPos.z - sampleDepth));
        occlusion       += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    outFragColor = vec4(occlusion, occlusion, occlusion, 1.0f);
}
