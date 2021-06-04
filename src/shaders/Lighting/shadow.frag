#version 330 core

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
    vec3 FragPos;
    vec4 FragPosLightSpace;
} vs_in;

// The final fragment output color
out vec4 FragColor ;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 normal = normalize(vs_in.normal);
    vec3 lightDir = normalize(lightPos - vs_in.FragPos);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1 - dot(normal, lightDir)), 0.05);
    // float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    float shadow = 0.0f;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -2; x <= 2; ++x)
    {
        for(int y = -2; y <= 2; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 25;

    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

void main()
{
    vec3 color = texture(diffuseTexture, vs_in.texCoords).rgb;
    vec3 normal = normalize(vs_in.normal);
    vec3 lightColor = vec3(1.0f);

    // ambient
    vec3 ambient = 0.25 * color;

    // diffuse
    vec3 lightDir = normalize(lightPos - vs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - vs_in.FragPos);
    float spec = 0.0f;
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
    vec3 specular = spec * lightColor;

    // calculate shadow
    float shadow = ShadowCalculation(vs_in.FragPosLightSpace);
    vec3 Lighting = (ambient + ((1.0 - shadow) * (diffuse + specular) * color));
    FragColor = vec4(Lighting, 1.0f);
}
