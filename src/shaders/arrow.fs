#version 450

in vec3 Color;
in float vdir;

out vec4 c;

//flat in int discard_flag;

void main(){
  // if(discard_flag == 1){
  //   discard;
  //   return;
  // }

  float bri = 1.0;
  if(vdir>0) bri = 0.7;

  c = vec4(Color*bri, 1.0f);
}
