#define GLSL(version, shader)  "#version " #version "\n" #shader


const char* FS_SOURCE = GLSL(130,                                                        
			     in vec3 Normal;
			     in vec3 WorldPos;
			     in vec3 Color;
			     out vec4 outColor;
			     struct BaseLight{
			       vec3 Color;
			       float Ambient;
			       float Diffuse;
			     }; 
			     struct PointLight{
			       vec3 Position;
			       BaseLight Base;
			     };			 
			     PointLight point_light;
			     uniform vec3 EyeWorldPos;
			     float MatSpecularIntensity = 1.1f;
			     int SpecularPower = 32;
			     vec4 computeLight(BaseLight light, vec3 Direction, vec3 Normal){
			       vec4 AmbientColor = vec4(light.Color, 1.0f) * light.Ambient;
			       float DiffuseFactor = max(dot(normalize(Normal), -Direction),0.0);
			       vec4 DiffuseColor = vec4(0, 0, 0, 0.0f);
			       vec4 SpecularColor = vec4(0, 0, 0, 0.0f);
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
			     void main() {

			       point_light.Base.Color = vec3(1.0f,1.0f,1.0f);
			       point_light.Base.Diffuse = 1.0f;
			       point_light.Base.Ambient = 0.25f;
			       outColor = vec4(Color,1) * computeLight(point_light.Base, vec3(1,-1,1), Normal); 
			       outColor.w = 1.0;
			     }   
);







const char* VS_SOURCE = GLSL(130, 
		  in vec3 in_vertex;
		  in vec3 pos;
		  in vec3 color;
		  in float scale;
		 uniform mat4 model;
		 uniform mat4 MVP;
		 out vec3 Normal;
		 out vec3 Color;
		 out vec3 WorldPos;
		 void main () {
		   vec4 temp = (vec4(scale*in_vertex.xyz+pos.xyz,1.0));
		   Color = color;
		   gl_Position = MVP*temp;
		   Normal = (model*vec4(scale*in_vertex,0.0)).xyz;
		   WorldPos = (model*temp).xyz;  
		 }
		 );




const char* LINE_VS_SOURCE = GLSL(130,                                                        
				  in vec3 in_vertex;
				  uniform mat4 MVP;
				  uniform float L;
				  void main() {                                               
				    gl_Position = MVP*(vec4((in_vertex.xyz-0.5)*L,1.0));
				  }        
				  );
  
const char* LINE_FS_SOURCE = GLSL(130,                                                        
				  out vec4 color;
				  void main() {                                               
				    color = vec4(0,0,1,1);
				  }        
				  );
