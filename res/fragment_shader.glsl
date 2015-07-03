#version 330


//in vec3 outcolor;

in vec3 Normal;
in vec3 WorldPos;
in vec3 Color;

out vec4 outColor;

struct BaseLight //Ambient + Diffuse
{
    vec3 Color;
    float Ambient;
    float Diffuse;
}; 


struct Atten_param
{
  float Constant, Linear, Exp;
};
struct PointLight
{
  vec3 Position;
  Atten_param Atten;
  BaseLight Base;
};



//uniform BaseLight light;
uniform PointLight point_light;

uniform vec3 EyeWorldPos;

uniform float MatSpecularIntensity; //Specular
uniform int SpecularPower;
uniform vec3 Directional_light; //Directional light




vec4 computeLight(BaseLight light, vec3 Direction, vec3 Normal)
{
  vec4 AmbientColor = vec4(light.Color, 1.0f) * light.Ambient;
  float DiffuseFactor = max(dot(normalize(Normal), -Direction),0.0);
  //outColor = texture(tex,Texcoord); 
    vec4 DiffuseColor = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0) {
        DiffuseColor = vec4(light.Color, 1.0f) * light.Diffuse * DiffuseFactor;
        vec3 VertexToEye = normalize(EyeWorldPos - WorldPos);
        vec3 LightReflect = normalize(reflect(Direction, Normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        SpecularFactor = pow(SpecularFactor, SpecularPower);
        if (SpecularFactor > 0) {
            SpecularColor = vec4(light.Color, 1.0f) * (MatSpecularIntensity * SpecularFactor);
        }
        
    }
    return (AmbientColor + DiffuseColor + SpecularColor);
}


vec4 computePointLight(vec3 Normal)
{
    vec3 LightDirection = WorldPos - point_light.Position;
    float Distance = distance(WorldPos,point_light.Position);
    LightDirection = normalize(LightDirection);

    vec4 Color = computeLight(point_light.Base, LightDirection, Normal);
    float Attenuation = point_light.Atten.Constant +
                        point_light.Atten.Linear * Distance +
                        point_light.Atten.Exp * Distance * Distance;

    return Color / (0.5*Attenuation);
} 



void main() {

  // vec4 TotalLight = computeLight(light, Directional_light, Normal);
   
   
 // vec4 TotalLight = computePointLight(Normal); 
    
   outColor = vec4(Color,1)* computePointLight(Normal);
} 
  

