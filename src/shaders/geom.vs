#version 450

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 pos;
layout(location = 2) in vec3 color;
layout(location = 3) in float scale;

uniform mat4 model;
uniform mat4 MVP;
uniform float pickscale;
uniform float gscale;

out vec3 Normal;
out vec3 Color;
out vec3 Pos;
flat out int id;
			     
void main () {
  id = gl_InstanceID+1;
  vec3 vpos = pickscale*scale*in_vertex*gscale;
  vec4 temp = vec4(vpos+pos*gscale, 1.0);
  Color = color;
  gl_Position =  MVP*temp;
  Normal = (model*vec4(vpos ,0.0)).xyz;
  Pos = (model*temp).xyz;  
}
