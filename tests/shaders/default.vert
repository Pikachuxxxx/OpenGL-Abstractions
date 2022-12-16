#version 410 core

layout (location = 0) in vec3 inPosition;
// layout (location = 1) in vec2 inTexCoords;

// out vec2 outTexCoords;

layout (std140) uniform VPMatrices
{
    mat4 u_View;
    mat4 u_Projection;
};

// uniform mat4 u_View;
// uniform mat4 u_Projection;
uniform mat4 u_Model;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(inPosition, 1.0);
    // outTexCoords = inTexCoords;
}
