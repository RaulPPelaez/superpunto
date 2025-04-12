#version 450
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 dir;
layout (location = 2) in vec3 color;


uniform mat4 MVP;

uniform float gscale;
uniform float dirscale;
out vec3 p;
out vec3 c;
out vec3 Dir;



void main () {
  
  vec4 temp = vec4(pos*gscale, 1.0);
  p = vec3(pos*gscale);
  c = color;
  Dir = dir;
  gl_Position =  MVP*temp;
}
