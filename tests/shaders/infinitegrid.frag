#version 330 core

float near = 0.01;
float far = 100.0;

in VS_OUT {
    vec3 nearPoint;
    vec3 farPoint;
    mat4 view;
    mat4 proj;
} vs_in;

out vec4 outFragColor;

vec4 grid(vec3 fragPos3D, float scale) {
    vec2 coord = fragPos3D.xz * scalne; // use the scale variable to set the distance between the lines
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    // z axis
    if(fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
        color.z = 1.0;
    // x axis
    if(fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
        color.x = 1.0;
    return color;
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = vs_in.proj * vs_in.view * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(vec3 pos) {
    vec4 clip_space_pos = vs_in.proj * vs_in.view * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
    return linearDepth / far; // normalize
}

void main()
{
    float t = -vs_in.nearPoint.y / (vs_in.farPoint.y - vs_in.nearPoint.y);
    vec3 fragPos3D = vs_in.nearPoint + t * (vs_in.farPoint - vs_in.nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));

    outFragColor = (grid(fragPos3D, 10) + grid(fragPos3D, 1)) * float(t > 0); // adding multiple resolution for the grid
    outFragColor.a *= fading;

    // outFragColor = vec4(1.0f, 0.0, 0.0, 1.0f * float(t > 0));
}
