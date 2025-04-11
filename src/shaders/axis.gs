#version 450

layout(points) in;
layout(line_strip, max_vertices = 2) out;

uniform mat4 MVP;

void main() {
    vec4 base = gl_in[0].gl_Position;
    vec4 tip = base + vec4(0.2, 0.0, 0.0, 0.0); // offset a bit along X

    gl_Position = MVP * base;
    EmitVertex();

    gl_Position = MVP * tip;
    EmitVertex();

    EndPrimitive();
}

// #version 450

// layout(points) in;
// layout(line_strip, max_vertices = 5) out;

// in vec3 basePos[];
// in vec3 direction[];
// flat in int axisID[];

// uniform mat4 MVP;

// out vec3 C;

// void main() {
//     vec3 base = basePos[0];
//     vec3 dir = direction[0];

//     // Arrowhead offset vector (scaled to look nice)
//     vec3 arrowOffset;
//     if (axisID[0] == 0) arrowOffset = vec3(0.0, 0.1, 0.0); // X
//     else if (axisID[0] == 1) arrowOffset = vec3(0.1, 0.0, 0.0); // Y
//     else arrowOffset = vec3(0.1, 0.0, 0.0); // Z

//     vec3 tip = base + dir;
//     vec3 color = vec3(axisID[0] == 0, axisID[0] == 1, axisID[0] == 2);

//     // Draw main axis line
//     gl_Position = MVP * vec4(base, 1.0);
//     C = color;
//     EmitVertex();

//     gl_Position = MVP * vec4(tip, 1.0);
//     C = color;
//     EmitVertex();

//     // Arrowhead side 1
//     gl_Position = MVP * vec4(tip - dir * 0.2 + arrowOffset, 1.0);
//     C = color;
//     EmitVertex();

//     // Back to tip
//     gl_Position = MVP * vec4(tip, 1.0);
//     C = color;
//     EmitVertex();

//     // Arrowhead side 2
//     gl_Position = MVP * vec4(tip - dir * 0.2 - arrowOffset, 1.0);
//     C = color;
//     EmitVertex();

//     EndPrimitive();
// }
