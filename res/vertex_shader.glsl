#version 130
/*
layout(location=0) in vec3 in_vertex; //cube vertex position
layout(location=3) in vec3 pos; //cube vertex position
layout(location=4) in vec3 color; //cube vertex position
layout(location=5) in float scale; //cube vertex position
*/
in vec3 in_vertex; //vertex position
in vec3 pos; //Instance position
in vec3 color; //Instanced color 
in float scale; //Instanced scale

uniform mat4 model;
uniform mat4 MVP;

//out vec3 outcolor;
out vec3 Normal;
out vec3 Color;
out vec3 WorldPos;

void main () {
  Color = color;
  gl_Position = MVP*(vec4(scale*in_vertex.xyz+pos.xyz,1.0));
  Normal = (model*vec4(scale*in_vertex,0.0)).xyz;
  WorldPos = (model*vec4(scale*in_vertex.xyz+pos.xyz,1.0)).xyz;
  
}
