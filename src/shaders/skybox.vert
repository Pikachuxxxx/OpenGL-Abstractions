#version 330 core

layout (location = 0) in vec3 position;

out VS_OUT {
    vec3 texCoords;
} vs_out;

layout (std140) uniform VPMatrices
{
    mat4 u_View;
    mat4 u_Projection;
};

uniform mat4 skyboxViewMatrix;
uniform mat4 skyboxProjectionMatrix;

void main()
{
    vs_out.texCoords = position;
    vec4 pos = skyboxProjectionMatrix * skyboxViewMatrix * vec4(position, 1.0f);
    gl_Position = pos.xyww;
}
