#version 330 core

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
    // vec4 FragPos;
} vs_in;

uniform sampler2D albedoMap;
uniform vec3 bodyLightColor;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

void main()
{
    // color = vec4(vs_in.texCoords, 0.0f, 1.0f); // Testing UV Coordinates
    FragColor = vec4(texture(albedoMap, vs_in.texCoords).xyz, 1.0f);
    // color = texture(texture_specular1, vs_in.texCoords);
    // // color = vec4(lightColor, 1.0f);
    // color = vec4(vs_in.FragPos.xyz, 1.0f); // Testing Normal cooridinates
    // color = vec4(1, 1, 1, 1);
    //color = vec4(vs_in.texCoords, 0.0f, 1.0f); // Testing Normal cooridinates

    FragColor = vec4(bodyLightColor, 1.0f);
    //FragColor = vec4(texture(albedoMap, vs_in.texCoords).xyz, 1.0f);

    // check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(bodyLightColor, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(bodyLightColor, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
