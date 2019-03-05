#version 450


layout (location = 0) in vec3 in_vertex;
layout (location = 1) in vec3 pos;
layout (location = 2) in vec3 color;
layout (location = 3) in float scale;

uniform mat4 model;
uniform mat4 normal_model;
uniform mat4 MVP;
uniform float pickscale = 1.0f;
uniform float gscale = 1.0f;

out vec3 Normal;
out vec3 Color;
out vec3 Pos;

flat out int id;

// mat4 rotationMatrix(vec3 axis, float angle){
//   axis = normalize(axis);
//   float s = sin(angle);
//   float c = cos(angle);
//   float oc = 1.0 - c;

//   return mat4(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0, oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
// 	      oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
// 	      0.0,                                0.0,                                0.0,                                1.0);
// }
float random(vec2 coordinate, int seed){
  return fract(sin(dot(coordinate*seed, vec2(12.9898, 78.233)))*43758.5453);
}
void main () {
  /*Ellipsoid*/
  // vec3 ss = vec3(1, 1, 2); //ratio of the ellipsoid radious, two of them must be equal
  // vec3 sss = vec3(0,0,1);  //Direction of the elongation
    
  // vec3 dir = vec3(1,0,1);  
  // vec3 gr = cross(sss, dir);    
  // float ang = acos(dot(dir,sss)/(length(sss)*length(dir)));    
  // mat4 rot = rotationMatrix(gr, ang);
      
  id = gl_InstanceID+1;
  // vec3 vpos = vec3(rot*vec4(pickscale*ss*in_vertex*gscale*scale,0));
  vec3 vpos = pickscale*in_vertex*gscale*scale;
  vec4 temp = vec4(vpos+pos*gscale, 1.0);
  Color = color;
  gl_Position =  MVP*temp;

  // vec2 coor = vec2(id*1000, 100*gl_VertexID);
  // vec3 rand = 10*vec3(random(coor,10132231), random(coor,101434232), random(coor,45101233));
  
  Normal =vec3(normal_model*vec4(vpos ,0.0));
  
  Pos = (model*temp).xyz;  
}
