#version 450
layout (location = 0) out vec4 outColor;
uniform vec3 color;

void main() {
  outColor = vec4(color, 1);
}   
