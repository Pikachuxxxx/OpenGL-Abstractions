#version 330 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
    vec4 FragPos;
} vs_in;

layout (location = 0) out vec4 FragColor;

uniform vec3 viewPos;
uniform vec3 objectColor;
uniform Material material;
uniform Light light;


float A = 0.1;
float B = 0.3;
float C = 0.6;
float D = 1.0;

float stepmix(float edge0, float edge1, float E, float x)
{
    float T = clamp(0.5 * (x - edge0 + E) / E, 0.0, 1.0);
    return mix(edge0, edge1, T);
}

vec4 toonShading(vec3 normal)
{
    vec3 dir_to_eye  = normalize(viewPos - vs_in.FragPos.xyz);
    vec3 half_vector = normalize(dir_to_eye - light.direction);
    
    float df = max(0.0, dot(normal, - light.direction));
    float sf = max(0.0, dot(normal, half_vector));
          sf = pow(sf, material.shininess);

    float E = fwidth(df);

    if      (df > A - E && df < A + E) df = stepmix(A, B, E, df);
    else if (df > B - E && df < B + E) df = stepmix(B, C, E, df);
    else if (df > C - E && df < C + E) df = stepmix(C, D, E, df);
    else if (df < A) df = 0.0;
    else if (df < B) df = B;
    else if (df < C) df = C;
    else df = D;

    E = fwidth(sf);
    if (sf > 0.5 - E && sf < 0.5 + E)
    {
        sf = smoothstep(0.5 - E, 0.5 + E, sf);
    }
    else
    {
        sf = step(0.5, sf);
    }

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, vs_in.texCoords));


   // Diffuse Lighting
    // vec3 lightDir = normalize(vs_in.lightPos - vs_in.FragPos); // Repalced using a constant light direction

    vec3 diffuse = light.diffuse * ( vec3(texture(material.diffuse, vs_in.texCoords)));

    // Specular shading

    vec3 specular = light.specular * ( vec3(texture(material.specular, vs_in.texCoords)));

    vec3 color = ambient + df * diffuse + sf * specular;

    return vec4(color, 1.0);
}

vec4 reinhard(vec4 hdr_color)
{
    // reinhard tonemapping
    vec3 ldr_color = hdr_color.rgb / (hdr_color.rgb + 1.0);

    // gamma correction
    ldr_color = pow(ldr_color, vec3(1.0 / 2.2));

    return vec4(ldr_color, 1.0);
}

void main()
{
    // Ambient lighting
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, vs_in.texCoords));

    // Diffuse Lighting
    vec3 normal = normalize(vs_in.normal);
    // vec3 lightDir = normalize(vs_in.lightPos - vs_in.FragPos); // Repalced using a constant light direction
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, vs_in.texCoords)));

    // Specular shading
    vec3 viewDir = normalize(viewPos - vs_in.FragPos.xyz);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, vs_in.texCoords)));

    // Emission shading
    vec3 emission = vec3(texture(material.emission, vs_in.texCoords));

    // Calculate the final lighting color
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0f);

    FragColor = reinhard(toonShading(normalize(normal)));
    //FragColor = vec4(vs_in.normal.xyz, 1.0f);
}
