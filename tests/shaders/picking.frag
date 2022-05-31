#version 330 core

uniform uint objectIndex;
uniform uint drawIndex;

out uvec3 outFragColor;

void main()
{
    outFragColor = uvec3(objectIndex, drawIndex, gl_PrimitiveID);
}