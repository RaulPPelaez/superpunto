#version 450
const vec2 v[4] =
  vec2[4]( vec2(-1.0, -1.0),
	   vec2( 1.0, -1.0),
	   vec2(-1.0,  1.0),
	   vec2( 1.0,  1.0));
out vec2 V;

void main () {
  gl_Position =  vec4(v[gl_VertexID], 0.0, 1.0);
  V = (v[gl_VertexID]+1)*0.5;
}
