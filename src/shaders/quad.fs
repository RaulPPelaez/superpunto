#version 450


uniform sampler2D ctex;

out vec4 c;
in vec2 V;
void main (){
  //c = texelFetch(ctex, ivec2(gl_FragCoord.xy), 0);
  c = texture(ctex, V);
  c.w = 1;
}
