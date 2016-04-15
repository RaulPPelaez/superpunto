#ifndef SHADERS_H
#define SHADERS_H

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



const char* VS_QUAD_SOURCE = GLSL(450,
      const vec2 v[4] =
	     vec2[4]( vec2(-1.0, -1.0),
		      vec2( 1.0, -1.0),
		      vec2(-1.0,  1.0),
		      vec2( 1.0,  1.0));
       void main () {
	 gl_Position =  vec4(v[gl_VertexID], 0.0, 1.0);
       }
);
const char* FS_QUAD_SOURCE = GLSL(450,
               layout(binding = 0) uniform sampler2D dtex;
               layout(binding = 1) uniform sampler2D tex;
	       out vec4 c;
	       void main (){
		 // float n = 1.0f;
		 // float f = 100.0f;
		 // float z = texture(dtex, gl_FragCoord.xy/800.0f).x;
		 // z =pow(z,2);//  (2.0f*n)/ (f+n-z*(f-n));
		 //c = vec4(z);


		 c = texelFetch(tex, ivec2(gl_FragCoord.xy), 0);
		  // if(textureSize(dtex, 0).x==1)
		  //   c = vec4(0,0.5,0,1);
		  // else
		  //   c = vec4(0,0,1,1);
		 
		 
		 
		  //c = texture(dtex, gl_FragCoord.xy/800.0f); 		 
		 //c = texture(tex, gl_FragCoord.xy/800.0f);
		 c.w = 1;
	       }
);



#endif
