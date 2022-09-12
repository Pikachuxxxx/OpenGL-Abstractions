#version 330 core

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
    vec3 FragPos;
    vec4 FragPosLightSpace;
} vs_in;

// The final fragment output color
out vec4 FragColor;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metalRoughnessMap;
uniform sampler2D shadowMap;

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectionalLight u_DirLight;

uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 normal = normalize(vs_in.normal);
    vec3 lightDir = normalize(u_DirLight.direction);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1 - dot(normal, lightDir)), 0.05);
    // float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

void main()
{
    vec3 color = texture(albedoMap, vs_in.texCoords).rgb;
    vec3 normal = normalize(vs_in.normal);

    // ambient
    vec3 ambient = 0.025 * color * u_DirLight.ambient;

    // diffuse
    vec3 lightDir = normalize(u_DirLight.direction);
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = diff * u_DirLight.diffuse;

    // Specular
    vec3 viewDir = normalize(viewPos - vs_in.FragPos);
    float spec = 0.0f;
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
    vec3 specular = spec * u_DirLight.specular;

    // calculate shadow
    float shadow = ShadowCalculation(vs_in.FragPosLightSpace);
    vec3 Lighting = (ambient + ((1.0 - shadow) * (diffuse + specular) * color));
    FragColor = vec4(Lighting, 1.0f);
}
