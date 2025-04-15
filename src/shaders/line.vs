#version 450

uniform mat4 MVP;
uniform vec3 pointA;
uniform vec3 pointB;

void main() {
  vec3 pos = (gl_VertexID == 0) ? pointA : pointB;
  gl_Position = MVP * vec4(pos, 1.0);
}
