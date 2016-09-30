#version 450

in vec3 C;
out vec4 c;

void main(){
  c = vec4(C,1);
}
