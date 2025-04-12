#version 450

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_color;

uniform mat4 MVP;

out vec3 C;

void main() {
    gl_Position = MVP * vec4(in_pos, 1.0);
    C = in_color;
}
