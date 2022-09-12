#version 450 core
#define MAX_POINT_LIGHTS 4
#define MAX_DIRECTIONAL_LIGHTS 4

struct Voxelization_Settings
{
	int use_ambient_light;
	int visualize_mipmap_level;
};

struct Directional_Light
{
	float strength;
	vec3 direction;
	vec3 color;
	vec3 attenuation;
};

layout(RGBA8) uniform image3D u_tex_voxelgrid;
uniform sampler2DShadow u_tex_shadowmap;

uniform Voxelization_Settings u_settings;
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metalRoughnessMap;

uniform int u_total_directional_lights;
uniform Directional_Light u_directional_lights[MAX_DIRECTIONAL_LIGHTS];

uniform vec3 u_scene_voxel_scale;

in vec3 f_normal;
in vec2 f_tex_coords;
in vec3 f_voxel_pos;
in vec4 f_shadow_coords;

vec4  f_albedo = vec4(0.0f);
float f_visibility = 1.0f; // calculated from shadow map

float attenuate(float dist, float strength, vec3 attenuation) {
	return strength / (attenuation.x + attenuation.y * dist + attenuation.z * dist * dist);
}

bool is_inside_clipspace(const vec3 p) {
	return abs(p.x) < 1 && abs(p.y) < 1 && abs(p.z) < 1;
}

vec3 from_clipspace_to_texcoords(vec3 p) {
	return 0.5f * p + vec3(0.5f); 
}

float calc_visibility()
{
	const float bias = 0.005f;
	return texture(u_tex_shadowmap, vec3(f_shadow_coords.xy, (f_shadow_coords.z - bias) / f_shadow_coords.w));
}

vec3 BRDF(vec3 lightDir, vec3 color, float strength, vec3 attenuation)
{
  float lightDst = length(lightDir);
  lightDir = lightDir / lightDst;
  float att = attenuate(lightDst, strength, attenuation);
  float diffuseFactor = max(dot(f_normal, lightDir), 0.0);
  vec3 scatteredLight = color * diffuseFactor * att;
  return scatteredLight;
}

vec3 calc_direct_light()
{
	vec3 color = vec3(0.0f);
	
	for (int i=0; i < u_total_directional_lights; i++) {
		Directional_Light light = u_directional_lights[i];
		color += (f_visibility * BRDF(normalize(light.direction), light.color, light.strength, light.attenuation));
	}

	return color;
}

void main()
{
	if (!is_inside_clipspace(f_voxel_pos))
		return;

	f_visibility = calc_visibility();
	f_albedo = texture(albedoMap, f_tex_coords);

	vec4 color = f_albedo * vec4(calc_direct_light(), 1.0f);
	color.a = 1.0;

	vec3 voxelgrid_tex_pos = from_clipspace_to_texcoords(f_voxel_pos);
	ivec3 voxelgrid_resolution = imageSize(u_tex_voxelgrid);
	imageStore(u_tex_voxelgrid, ivec3(voxelgrid_resolution * voxelgrid_tex_pos), color);
}