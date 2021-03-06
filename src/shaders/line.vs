#version 450

layout (location = 0) in vec3 in_vertex;

uniform mat4 model;
uniform mat4 MVP;
uniform float gscale = 1.0f;

void main () {
  vec3 vpos = in_vertex*gscale;
  vec4 temp = vec4(vpos, 1.0);
  gl_Position =  MVP*temp;
}
