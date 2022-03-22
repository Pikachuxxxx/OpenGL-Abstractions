#version 450 core

in VERTEX_OUT{
	vec3 fragmentPosition;
	vec2 textureCoordinates;
	mat3 TBN;
	vec3 tangentViewPosition;
	vec3 tangentFragmentPosition;
} fragment_in;

struct PointLight {
	vec4 color;
	vec4 position;
	vec4 paddingAndRadius;
};

struct VisibleIndex {
	int index;
};

// Shader storage buffer objects
layout(std430, binding = 0) readonly buffer LightBuffer{
	PointLight data[];
} lightBuffer;

layout(std430, binding = 1) readonly buffer VisibleLightIndicesBuffer{
	VisibleIndex data[];
} visibleLightIndicesBuffer;

uniform int numberOfTilesX;
uniform int totalLightCount;

out vec4 fragColor;

const vec4 kRadarColors[12] =
{
	vec4(0.0,0.0,0.2,0.2), // light blue
	vec4(0.0,0.0,0.6,0.5), // dark blue
	vec4(0.0,0.0,0.9,0.5), // blue
	vec4(0.0,0.6,0.9,0.5), // light blue
	vec4(0.0,0.9,0.9,0.5), // cyan
	vec4(0.0,0.9,0.6,0.5), // blueish green
	vec4(0.0,0.9,0.0,0.5), // green
	vec4(0.6,0.9,0.0,0.5), // yellowish green
	vec4(0.9,0.9,0.0,0.5), // yellow
	vec4(0.9,0.6,0.0,0.5), // orange
	vec4(0.9,0.0,0.0,0.5), // red
	vec4(1.0,0.0,0.0,0.7) // strong red
};

void main() {
	// Determine which tile this pixel belongs to
	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	uint index = tileID.y * numberOfTilesX + tileID.x;

	uint offset = index * 1024;
	uint i;
	for (i = 0; i < 1024 && visibleLightIndicesBuffer.data[offset + i].index != -1; i++);

	float fMaxNrLightsPerTile = 24;
	// change of base
	// logb(x) = log2(x) / log2(b)
	//int nColorIndex = i==0 ? 0 : (1 + (int) floor(10 * (log2((float)i) / log2(fMaxNrLightsPerTile))));

	int nColorIndex =  i==0 ? 0 : (1 + int(floor(10 * log2(float(i)) / log2(fMaxNrLightsPerTile))));
	nColorIndex = nColorIndex<0 ? 0 : nColorIndex;
	vec4 col = nColorIndex>11 ? vec4(1.0,1.0,1.0,0.8) : kRadarColors[nColorIndex];

	//float ratio = float(i) / float(totalLightCount);
	fragColor = col;
}