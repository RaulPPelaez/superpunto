#version 450

uniform sampler2D ctex;
uniform sampler2D ndtex;
uniform sampler2D ptex;
uniform sampler2D SSAOtex;

layout(location = 0) out vec4 c;
struct Light{
  vec3 pos;
  vec3 color;
};

uniform Light light = {vec3(0, 0, 0), vec3(1, 1, 1)};
uniform vec3 viewPos;
uniform float ambient = 0.3f;
uniform float diffuseFactor = 0.4f;
uniform float specularFactor = 0.1f;
uniform float shininess = 16.0f;

void main (){
  vec3 FragPos = texelFetch(ptex, ivec2(gl_FragCoord.xy), 0).xyz;
  vec4 Normal  = texelFetch(ndtex, ivec2(gl_FragCoord.xy), 0).xyzw;
  vec3 Color   = texelFetch(ctex, ivec2(gl_FragCoord.xy), 0).rgb;
  float Occlusion = texelFetch(SSAOtex, ivec2(gl_FragCoord.xy), 0).r;
  if(Normal.w==1.0f){
    c = vec4(Color,1.0f);
    return;
  }
  float occ = pow(Occlusion, 1.2)*1.6f;
  vec3 lighting = vec3(0);
  vec3 viewDir = (normalize(viewPos - FragPos));
  //Light comes from camera
  vec3 lightDir= (normalize(viewDir));
  vec3 diffuse = max(dot(Normal.xyz, lightDir), 0.0f)*Color*light.color*diffuseFactor;
  //Blinn-Phong
  vec3 hwdir = normalize(lightDir+viewDir);
  float spec = pow(max(dot(Normal.xyz, hwdir), 0.0f), shininess);
  vec3 specular = light.color*(spec*specularFactor);
  lighting += diffuse + specular;
  lighting = lighting*occ + Color*ambient*occ;
  c = vec4(lighting, 1.0f);
}
