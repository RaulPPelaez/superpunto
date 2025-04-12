#version 450
in vec3 Normal;
in vec3 Pos;
in vec3 Color;
flat in int id;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 normalDepth;
layout(location = 2) out vec3 positionBuffer;

uniform bool picking;
uniform bool drawing_picked;

uniform float znear;
uniform float zfar;
void main() {
  if(picking){
    //id*2 to gain precision, "only" 255^3/2 differenciable objects
    int r = (id*2)%256;
    int g = (id*2)/256%256;
    int b = (id*2)/(256*256);
    outColor = vec4(r/256.0f,g/256.0f,b/256.0f, 1.0f);
  }
  else if(drawing_picked){
    outColor = vec4(1.0f-Color, 1.0f);
  }
  else{
    outColor = vec4(Color,1);
    outColor.w = 1.0f;
  }
  float z = 2.0f*gl_FragCoord.z-1.0f;
  positionBuffer = Pos;
  /*Linearized depth encoded between 0 and 1*/
  float linearDepth = 2.0f*znear*zfar/(zfar+znear-z*(zfar-znear));
  normalDepth = vec4(normalize(Normal),
		     (linearDepth-znear)/(zfar-znear));

}
