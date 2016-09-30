#version 450


const vec3 v[3] =
  vec3[3]( vec3(1, 0,0),
	   vec3(0,1,0),
	   vec3(0,0,1));


out vec3 dir;
out vec3 c;
out int axis;
uniform mat4 MVP;
uniform float axislength = 1.0f;
uniform vec3 origin;

void main () {
  axis = gl_VertexID;
  gl_Position =  MVP*vec4(origin, 1.0);
  dir = axislength*v[gl_VertexID];
  c = vec3(gl_VertexID==0?1:0, gl_VertexID==1?1:0, gl_VertexID==2?1:0);
}
