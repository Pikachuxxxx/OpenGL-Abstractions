#version 330 core

//out vec4 outFragColor;

//in VS_OUT {
//    vec3 normal;
//    vec2 texCoords;
//} vs_in;

void main()
{
    gl_FragDepth = gl_FragCoord.z;
    //outFragColor = vec4(vs_in.normal, 1.0f);
}
