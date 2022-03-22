#version 330 core

out VS_OUT {
    vec3 nearPoint;
    vec3 farPoint;
    mat4 view;
    mat4 proj;
} vs_out;

layout (std140) uniform VPMatrices
{
    mat4 u_View;
    mat4 u_Projection;
};

// Grid position are in xy clipped space
vec3 gridPlane[6] = vec3[6](
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
    vec3 p = gridPlane[gl_VertexID].xyz;
    vs_out.nearPoint = UnprojectPoint(p.x, p.y, 0.0, u_View, u_Projection).xyz; // unprojecting on the near plane
    vs_out.farPoint = UnprojectPoint(p.x, p.y, 1.0, u_View, u_Projection).xyz; // unprojecting on the far plane
    vs_out.view = u_View;
    vs_out.proj = u_Projection;
    gl_Position = vec4(p, 1.0); // using directly the clipped coordinates
}
