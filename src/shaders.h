#define GLSL(version, shader)  "#version " #version "\n" #shader


const char* FS_SOURCE = GLSL(450,                     
			     in vec3 Normal;
			     in vec3 WorldPos;
			     in vec3 Color;
			     out vec4 outColor;

			     uniform vec3 EyeWorldPos;

			     struct BaseLight{
			       vec3 Color;
			       float Ambient;
			       float Diffuse;
			     }; 
			     BaseLight light;
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

			       light.Color = vec3(1.0f,1.0f,1.0f);
			       light.Diffuse = 1.0f;
			       light.Ambient = 0.25f;
			       outColor = vec4(Color,1) * computeLight(light, vec3(1,-1,1), Normal); 
			       //outColor = vec4(1,0,0,0);
			       outColor.w = 1.0f;
			     }   
);







const char* VS_SOURCE = GLSL(450, 
			     layout (location = 0) in vec3 in_vertex;
			     layout (location = 1) in vec3 pos;
			     layout (location = 2) in vec3 color;
			     layout (location = 3) in float scale;
			     uniform mat4 model;
			     uniform mat4 MVP;
			     out vec3 Normal;
			     out vec3 Color;
			     out vec3 WorldPos;
			     void main () {
			       vec3 vpos = scale*in_vertex;
			       vec4 temp = vec4(vpos+pos, 1.0);
			       Color = color;
			       gl_Position =  MVP*temp;
			       Normal = (model*vec4(vpos ,0.0)).xyz;
			       WorldPos = (model*temp).xyz;  
			     }
		 );




const char* VS_KK_SOURCE = GLSL(450, 	
			       const vec4 v[3] = vec4[3]( vec4(0.25, -0.25, 0.5, 1),
							  vec4(-0.25, -0.25, 0.5, 1),
							  vec4(0.25, 0.25, 0.5, 1));

			     void main () {
			       gl_Position =  v[gl_VertexID];
			     }
		 );
const char* FS_KK_SOURCE = GLSL(450, 			   
			       out vec4 c;
			     void main () {
			       c = vec4(1.0f,0.0f,0.0f,1.0f);
			     }
		 );


