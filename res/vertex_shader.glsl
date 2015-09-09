#version 330
layout (location = 0) in vec3 in_vertex; //vertex position
layout (location = 1) in vec3 pos; //Instance position
layout (location = 2) in vec3 color; //Instanced color 
layout (location = 3) in float scale; //Instanced scale

uniform mat4 model;
uniform mat4 MVP;
uniform mat4 depthBiasMVP;

//out vec3 outcolor;
out vec3 Normal;
out vec3 Color;
out vec3 WorldPos;
//out vec4 ShadowCoord;

void main () {
  vec4 temp = (vec4(scale*in_vertex.xyz+pos.xyz,1.0));
  Color = color;
  gl_Position = MVP*temp;
  Normal = (model*vec4(scale*in_vertex,0.0)).xyz;
  WorldPos = (model*vec4(scale*in_vertex.xyz+pos.xyz,1.0)).xyz;  
  //ShadowCoord = depthBiasMVP * temp;
}
