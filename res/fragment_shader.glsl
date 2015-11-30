#version 420


in vec3 Normal;
in vec3 WorldPos;
in vec3 Color;
in vec4 ShadowCoord;

out vec4 outColor;

layout (binding = 0) uniform sampler2DShadow shadowmap;

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
//uniform vec3 Directional_light; //Directional light




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
    /*
    float Attenuation = point_light.Atten.Constant +
                        point_light.Atten.Linear * Distance +
                        point_light.Atten.Exp * Distance * Distance;
    */
    return Color;
} 



void main() {

  //  float extras = 1.0;

    //outColor = vec4(Color,1) * computePointLight(Normal)*extras;
    outColor = vec4(Color,1) * computeLight(point_light.Base, vec3(1,-1,1), Normal); 
    //outColor.w = 1.0;
    //float vs = textureProj( shadowmap, ShadowCoord );
    //outColor = (vs+0.1)*vec4(Color,1);
    outColor.w = 1.0;

} 
  

