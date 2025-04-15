#version 450

uniform sampler2D ctex;

out vec4 c;

in vec2 vv;
void main (){
  vec2 vvv = vv;
  vvv.y = -vv.y;
  c = texture(ctex,  vvv);
}
