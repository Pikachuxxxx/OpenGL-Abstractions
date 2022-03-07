#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

layout (std140) uniform VPMatrices
{
    mat4 u_View;
    mat4 u_Projection;
};

uniform mat4 u_Model;

//out VS_OUT {
//    vec3 normal;
//    vec2 texCoords;
//} vs_out;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0);

    //vs_out.normal = normal;
    //vs_out.texCoords = texCoords;
}
