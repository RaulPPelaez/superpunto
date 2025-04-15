#version 450

layout (location = 0) out vec4 outColor;
layout(location = 1) out vec4 normalDepth;
uniform vec3 color;

void main() {
  outColor = vec4(color, 1.0f);
  //Excludes lines from SSAO effects
  normalDepth = vec4(0,0,0,1);
}
