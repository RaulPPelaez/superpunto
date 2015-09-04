#version 440
layout(vertices = 3) out;
in vec3 vPosition[];
out vec3 tcPosition[];
#define ID gl_InvocationID
void main(){
  tcPosition[ID] = vPosition[ID];
  if (ID == 0) {
    gl_TessLevelInner[0] = 4;
    gl_TessLevelOuter[0] = 4;
    gl_TessLevelOuter[1] = 4;
    gl_TessLevelOuter[2] = 4;
  }

}
