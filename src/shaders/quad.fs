#version 450


uniform sampler2D ctex;

out vec4 c;

void main (){
  c = texelFetch(ctex, ivec2(gl_FragCoord.xy), 0);
  c.w = 1;
}
