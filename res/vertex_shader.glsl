#version 430 core


layout(location=0) in vec3 in_vertex; //cube vertex position
layout(location=2) in vec3 normal; //cube vertex position
layout(location=3) in vec3 pos; //cube vertex position
layout(location=4) in vec3 color; //cube vertex position
layout(location=5) in float scale; //cube vertex position





uniform mat4 model;
uniform mat4 MVP;

//out vec3 outcolor;
out vec3 Normal;
out vec3 Color;
out vec3 WorldPos;

void main () {
  //outcolor = vec3(0,0,0);
  Color = color;
  gl_Position = MVP*(vec4(scale*in_vertex.xyz+pos.xyz,1.0));
  Normal = (model*vec4(normal,0.0)).xyz;
  //outcolor = Normal;
  WorldPos = (model*vec4(scale*in_vertex.xyz+pos.xyz,1.0)).xyz;
  
}