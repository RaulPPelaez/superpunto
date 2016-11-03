#version 450

uniform sampler2D ctex;

//uniform sampler2D dtex;
uniform sampler2D ndtex;
uniform sampler2D ptex;
//uniform sampler2D noisetex;
uniform sampler2D SSAOtex;

layout(location = 0) out vec4 c;
struct Light{
  vec3 pos;
  vec3 color;
};

uniform Light light = {vec3(0, 0, 0), vec3(1, 1, 1)};
uniform vec3 viewPos;
uniform float ambient = 0.3f;//*vec3(0.25f, 0.20725f, 0.20725f);
uniform float diffuseFactor = 0.4;// vec3(1.0f, 0.829f, 0.829f);
uniform float specularFactor = 0.1;//vec3(0.296648f, 0.296648f, 0.296648f);
uniform float shininess = 16.0f;

void main (){

  //float occ = texelFetch(SSAOtex, ivec2(gl_FragCoord.xy), 0).r;
  // c = texture(noisetex, gl_FragCoord.xy/800, 0);
  // c.w = 1;
  // return;
  vec3 FragPos = texelFetch(ptex, ivec2(gl_FragCoord.xy), 0).xyz;
  vec4 Normal  = texelFetch(ndtex, ivec2(gl_FragCoord.xy), 0).xyzw;
  vec3 Color   = texelFetch(ctex, ivec2(gl_FragCoord.xy), 0).rgb;
  //  float Specular   = texelFetch(ctex, ivec2(gl_FragCoord.xy), 0).w;
  float Occlusion = texelFetch(SSAOtex, ivec2(gl_FragCoord.xy), 0).r;
  //TODO fix this, when there is no geom at this pixel
  if(Normal.w==1.0f){
    c = vec4(Color,1.0f);
    return;
  }
  //c = vec4(Occlusion);
  // return;
  float occ = pow(Occlusion, 1.2)*1.6f;
  vec3 lighting = vec3(0);//Color*ambient);

  vec3 viewDir = normalize(viewPos - FragPos);

  /*Light comes from camera*/
  vec3 lightDir= normalize(viewDir); //normalize(light.pos - FragPos); //Fixed light
  vec3 diffuse = max(dot(Normal.xyz, lightDir), 0.0)*Color*light.color*diffuseFactor;

  // vec3 reflectDir = reflect(-lightDir, Normal.xyz);
  // float spec = pow( max(dot(viewDir, reflectDir), 0.0), shininess);

  //Blinn-Phong
  vec3 hwdir = normalize(lightDir+viewDir);
  float spec = pow(max(dot(Normal.xyz, hwdir), 0.0), shininess);

  vec3 specular = light.color*(spec*specularFactor);



  // float dist = length(viewPos - FragPos);
  // float attenuation = 1.0 / (1.0 + 0.0005 * dist + 0.00001 * dist * dist);

  lighting += diffuse + specular;

  lighting = lighting*occ + Color*ambient*occ;
 
  //  float z = Normal.w;
  c = vec4(lighting, 1.0f);
  //c = vec4(occ,occ,occ, 1.0f);

}
