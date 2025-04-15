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
  outColor = vec4(Color,1);
  float z = 2.0f*gl_FragCoord.z-1.0f;
  positionBuffer = Pos;
  //Linearized depth encoded between 0 and 1
  float linearDepth = 2.0f*znear*zfar/(zfar+znear-z*(zfar-znear));
  normalDepth = vec4(normalize(Normal),
		     (linearDepth-znear)/(zfar-znear));
  if(picking){
    const uint uid = uint(id);
    const uvec4 bytes = uvec4(
			      (uid >> 0)  & 0xFF,
			      (uid >> 8)  & 0xFF,
			      (uid >> 16) & 0xFF,
			      (uid >> 24) & 0xFF
    );
    outColor = vec4(bytes) / 255.0;
  }
  else if(drawing_picked){
    outColor = vec4(1.0f-Color, 1.0f);
    //Excludes lines from SSAO effects
    normalDepth = vec4(0,0,0,1);
  }

}
