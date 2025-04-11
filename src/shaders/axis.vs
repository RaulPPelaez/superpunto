#version 450

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_color;

uniform mat4 MVP;

out vec3 C;

void main() {
    gl_Position = MVP * vec4(in_pos, 1.0);
    C = in_color;
}

// #version 450

// // Outputs to geometry shader
// out vec3 basePos;
// out vec3 direction;
// flat out int axisID;

// uniform vec3 origin;
// uniform float axislength;

// void main() {
//     // Axis directions: X, Y, Z
//     vec3 axes[3] = vec3[](
//         vec3(1.0, 0.0, 0.0), // X
//         vec3(0.0, 1.0, 0.0), // Y
//         vec3(0.0, 0.0, 1.0)  // Z
//     );

//     axisID = gl_VertexID;
//     direction = axes[gl_VertexID] * axislength;
//     basePos = origin;
// }
