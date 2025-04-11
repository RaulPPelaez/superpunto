#version 450

in vec3 C;
out vec4 fragColor;

void main() {
    fragColor = vec4(C, 1.0); // RGB axis colors
}
