#ifndef SHADERS_H
#define SHADERS_H

#define GLSL(version, shader)  "#version " #version "\n" #shader


const char* FS_SOURCE = GLSL(450,                     
			     in vec3 Normal;
			     in vec3 WorldPos;
			     in vec3 Color;
			     in flat int id;
			     layout (location = 0) out vec4 outColor;
			     layout (location = 1) out vec4 normalDepth;

			     
			     uniform bool picking;
			     uniform bool drawing_picked;

			     uniform float znear;
			     uniform float zfar;

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
			        if(picking){
				  //id*2 to gain precision, "only" 255^3/2 differenciable objects 
				  int r = (id*2)%256;
				  int g = (id*2)/256%256;
				  int b = (id*2)/(256*256);
				  outColor = vec4(r/256.0f,g/256.0f,b/256.0f,1.0f);       
			        }
			        else if(drawing_picked){
				  outColor = vec4(1.0f-Color,1);
				}
				else{
				 light.Color = vec3(1.0f,1.0f,1.0f);
				 light.Diffuse = 1.0f;
				 light.Ambient = 0.25f;
				 outColor = mix(vec4(0.0f),vec4(Color,1), computeLight(light, vec3(1,-1,1), Normal));
				 //outColor = vec4(1,0,0,0);
				 outColor.w = 1.0f;
				}
				//				float z = gl_FragCoord.z*2.0f-1.0f;
				float z = gl_FragCoord.z;
				float linearDepth = 2.0f*znear*zfar/(zfar+znear-z*(zfar-znear));
				normalDepth = vec4(Normal, linearDepth);

			     }   
);







const char* VS_SOURCE = GLSL(450, 
			     layout (location = 0) in vec3 in_vertex;
			     layout (location = 1) in vec3 pos;
			     layout (location = 2) in vec3 color;
			     layout (location = 3) in float scale;
			     uniform mat4 model;
			     uniform mat4 MVP;
			     uniform float pickscale;
			     out vec3 Normal;
			     out vec3 Color;
			     out vec3 WorldPos;
			     out int id;
			     
			     void main () {
			       id = gl_InstanceID+1;
			       vec3 vpos = pickscale*scale*in_vertex;
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
               layout(binding = 1) uniform sampler2D ctex;
               layout(binding = 2) uniform sampler2D ndtex;
	       out vec4 c;
	       void main (){
		 //Uncomment for depth view
		  // float n = 1.0f;
		  // float f = 100.0f;
		  // float z = texture(dtex, gl_FragCoord.xy/800.0f).x;
		  // z =pow(z,2);//  (2.0f*n)/ (f+n-z*(f-n));
		 //c = vec4(z);


		 c = texelFetch(ctex, ivec2(gl_FragCoord.xy), 0);

		  // if(textureSize(dtex, 0).x==1)
		  //   c = vec4(0,0.5,0,1);
		  // else
		  //   c = vec4(0,0,1,1);
		 		 
		  //c = texture(dtex, gl_FragCoord.xy/800.0f); 		 
		 //c = texture(tex, gl_FragCoord.xy/800.0f);
		 c.w = 1;
	       }
);


//TODO ARREGLAR ESTO
const char* FS_SSAO_SOURCE = GLSL(450,
	       layout(binding = 0) uniform sampler2D dtex;
               layout(binding = 1) uniform sampler2D sColor;
               layout(binding = 2) uniform sampler2D sNormalDepth;
	       out vec4 color;


// Various uniforms controling SSAO effect
 float ssao_level = 0.3f;
 float object_level = 1.0f;
 float ssao_radius = 0.05f*800.0f/1000.0f;
 bool weight_by_angle = true;
 uint point_count = 16;
 bool randomize_points = true;

// Uniform block containing up to 256 random directions (x,y,z,0)
// and 256 more completely random vectors
layout (binding = 0, std140) uniform SAMPLE_POINTS
{
    vec4 pos[256];
    vec4 random_vectors[256];
} points;

void main(void)
{
    // Get texture position from gl_FragCoord
    vec2 P = gl_FragCoord.xy / textureSize(sNormalDepth, 0);
    // ND = normal and depth
    vec4 ND = textureLod(sNormalDepth, P, 0);
    // Extract normal and depth
    vec3 N = ND.xyz;
    float my_depth = ND.w;

    // Local temporary variables
    int i;
    int j;
    int n;

    float occ = 0.0;
    float total = 0.0;

    // n is a pseudo-random number generated from fragment coordinate
    // and depth
    n = (int(gl_FragCoord.x * 7123.2315 + 125.232) *
         int(gl_FragCoord.y * 3137.1519 + 234.8)) ^
         int(my_depth);
    // Pull one of the random vectors
    vec4 v = points.random_vectors[n & 255];

    // r is our 'radius randomizer'
    float r = (v.r + 3.0) * 0.1;
    if (!randomize_points)
        r = 0.5;

    // For each random point (or direction)...
    for (i = 0; i < point_count; i++)
    {
        // Get direction
        vec3 dir = points.pos[i].xyz;

        // Put it into the correct hemisphere
        if (dot(N, dir) < 0.0)
            dir = -dir;

        // f is the distance we've stepped in this direction
        // z is the interpolated depth
        float f = 0.0;
        float z = my_depth;

        // We're going to take 4 steps - we could make this
        // configurable
        total += 4.0;

        for (j = 0; j < 4; j++)
        {
            // Step in the right direction
            f += r;
            // Step _towards_ viewer reduces z
            z -= dir.z * f;

            // Read depth from current fragment
            float their_depth =
                textureLod(sNormalDepth,
                           (P + dir.xy * f * ssao_radius), 0).w;

            // Calculate a weighting (d) for this fragment's
            // contribution to occlusion
            float d = abs(their_depth - my_depth);
            d *= d;

            // If we're obscured, accumulate occlusion
            if ((z - their_depth) > 0.0)
            {
                occ += 4.0 / (1.0 + d);
            }
        }
    }

    // Calculate occlusion amount
    float ao_amount = (1.0 - occ / total);

    // Get object color from color texture
    vec4 object_color =  textureLod(sColor, P, 0);

    // Mix in ambient color scaled by SSAO level
    color = object_level * object_color +
      mix(vec4(0.2), vec4(ao_amount), ssao_level);
}


);



#endif




  // //TODO MIERDA
  // GLuint points_buffer;
  // SAMPLE_POINTS point_data;
  // int i;

  // for (i = 0; i < 256; i++)                                                  
  //   {                                                                          
  //       do                                                                     
  // 	  {                                                                      
  //           point_data.point[i][0] = random_float() * 2.0f - 1.0f;             
  //           point_data.point[i][1] = random_float() * 2.0f - 1.0f;             
  //           point_data.point[i][2] = random_float(); //  * 2.0f - 1.0f;        
  //           point_data.point[i][3] = 0.0f;                                     
  // 	  } while (length(point_data.point[i]) > 1.0f);                          
  //       normalize(point_data.point[i]);                                        
  //   }                                                                          
  // for (i = 0; i < 256; i++)                                                  
  //   {                                                                          
  //     point_data.random_vectors[i][0] = random_float();                      
  //     point_data.random_vectors[i][1] = random_float();                      
  //     point_data.random_vectors[i][2] = random_float();                      
  //     point_data.random_vectors[i][3] = random_float();                      
  //   }          



  // glGenBuffers(1, &points_buffer);
  // glBindBuffer(GL_UNIFORM_BUFFER, points_buffer);
  // glBufferData(GL_UNIFORM_BUFFER, sizeof(SAMPLE_POINTS), &point_data, GL_STATIC_DRAW);
  // glBindBufferBase(GL_UNIFORM_BUFFER, 0, points_buffer);
