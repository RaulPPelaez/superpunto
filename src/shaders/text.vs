#version 450
uniform vec2 size;
uniform vec2 pos;
const vec2 v[4] =
  vec2[4]( vec2(-1.0, -1.0),
	   vec2( 1.0, -1.0),
	   vec2(-1.0,  1.0),
	   vec2( 1.0,  1.0));


out vec2 vv;
void main () {
  int id = gl_VertexID;
  vec2 ori = vec2(-1,1);
  vv = (v[id]+1)/2;
  vec2 vpos = ((v[gl_VertexID]+ori)/2  - ori)*size+ori;

  gl_Position =  vec4(vpos+2*pos/vec2(800,-600), 0.0, 1.0);
}
