#define GLSL(version, shader)  "#version " #version "\n" #shader


const char* VS_SOURCE = GLSL(450, 
layout (location = 0) in vec3 in_vertex;
layout (location = 2) in vec3 color;
layout (location = 3) in float scale;
uniform mat4 model;
uniform mat4 proj;
uniform mat4 MVP;
uniform vec4 viewport;
	  uniform float pointScale;

out vec3 Color;
out vec2 center;
out float R;

void main(){

  vec3 posEye = vec3( model * vec4(in_vertex,1.0));
  float dist = length(posEye);

  R = 100*scale*(pointScale/dist);
  vec4 temp = vec4(in_vertex, 1.0);

  gl_Position =  MVP*temp;

  Color = color;
  gl_PointSize = R;
  center = gl_Position.xy;  

}
);


const char* FS_SOURCE = GLSL(450,                     
in vec3 Color;
in float R;

in vec2 center;
out vec4 outColor;

			     uniform mat4 model;
			     uniform mat4 MVP;
uniform vec4 viewport;
uniform mat4 proj;


void main(){
  
  vec3 lightDir = vec3(0.577, 0.577, 0.577);

  vec3 N;

  N.xy = gl_PointCoord.xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);

  float mag = dot(N.xy, N.xy);
  


  if(mag>1.0) discard;//outColor = vec4(0,1,0,1);
  else{
    
    
    N.z = sqrt(1.0-mag);
    float diffuse = max(0.0, dot(lightDir, N));
    outColor = vec4(Color,1)*diffuse+0.1*vec4(1,1,1,1);
  }
}
);

/* PERSPECTIVE




const char* VS_SOURCE = GLSL(450, 
layout (location = 0) in vec3 in_vertex;
layout (location = 2) in vec3 color;
layout (location = 3) in float scale;
uniform mat4 model;
uniform mat4 proj;
uniform mat4 MVP;
uniform vec4 viewport;

out vec3 Color;
out mat4 VPMTInverse;
out mat4 VPInverse;
out vec3 centernormclip;

out float R;
void main(){
  R = scale*10;
  vec4 temp = vec4(in_vertex, 1.0);
  gl_Position =  MVP*temp;
  Color = color;


  mat4 T = mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		in_vertex.x/R, in_vertex.y/R, in_vertex.z/R, 1.0/R);

  mat4 PMTt = transpose(MVP * T);

  vec4 r1 = PMTt[0];
  vec4 r2 = PMTt[1];
  vec4 r4 = PMTt[3];
  float r1Dr4T = dot(r1.xyz,r4.xyz)-r1.w*r4.w;
  float r1Dr1T = dot(r1.xyz,r1.xyz)-r1.w*r1.w;
  float r4Dr4T = dot(r4.xyz,r4.xyz)-r4.w*r4.w;
  float r2Dr2T = dot(r2.xyz,r2.xyz)-r2.w*r2.w;
  float r2Dr4T = dot(r2.xyz,r4.xyz)-r2.w*r4.w;

  gl_Position = vec4(-r1Dr4T, -r2Dr4T, gl_Position.z/gl_Position.w*(-r4Dr4T), -r4Dr4T);


  float discriminant_x = r1Dr4T*r1Dr4T-r4Dr4T*r1Dr1T;
  float discriminant_y = r2Dr4T*r2Dr4T-r4Dr4T*r2Dr2T;
  float screen = max(viewport.z, viewport.w);

  gl_PointSize = sqrt(max(discriminant_x, discriminant_y))*screen/(-r4Dr4T);


  // prepare varyings

  mat4 TInverse = mat4(
		       1.0,          0.0,          0.0,         0.0,
		       0.0,          1.0,          0.0,         0.0,
		       0.0,          0.0,          1.0,         0.0,
		       -in_vertex.x, -in_vertex.y, -in_vertex.z, R);
  mat4 VInverse = mat4( // TODO: move this one to CPU
		       2.0/viewport.z, 0.0, 0.0, 0.0,
		       0.0, 2.0/viewport.w, 0.0, 0.0,
		       0.0, 0.0, 2.0, 0.0,

		       -(viewport.z+2.0*viewport.x)/viewport.z,
		       -(viewport.w+2.0*viewport.y)/viewport.w,
		       -0.5f, 1.0);

  VPMTInverse = TInverse*inverse(MVP)*VInverse;
  VPInverse = inverse(proj)*VInverse; // TODO: move to CPU
  vec4 centerclip = MVP*temp;
  centernormclip = vec3(centerclip)/centerclip.w;



}
);






const char* FS_SOURCE = GLSL(450,                     

in vec3 Color;

in mat4 VPMTInverse;
in mat4 VPInverse;
in vec3 centernormclip;

out vec4 outColor;

uniform vec4 viewport;
uniform mat4 proj;


void main(){
    vec4 c3 = VPMTInverse[2];
    vec4 xpPrime = VPMTInverse*vec4(gl_FragCoord.x, gl_FragCoord.y, 0.0, 1.0);

    float c3TDc3 = dot(c3.xyz, c3.xyz)-c3.w*c3.w;
    float xpPrimeTDc3 = dot(xpPrime.xyz, c3.xyz)-xpPrime.w*c3.w;
    float xpPrimeTDxpPrime = dot(xpPrime.xyz, xpPrime.xyz)-xpPrime.w*xpPrime.w;

    float square = xpPrimeTDc3*xpPrimeTDc3 - c3TDc3*xpPrimeTDxpPrime;
    if (square<0.0) {
        discard;
    } else {
        float z = ((-xpPrimeTDc3-sqrt(square))/c3TDc3);
        gl_FragDepth = z;

        vec4 pointclip = VPInverse*vec4(gl_FragCoord.x, gl_FragCoord.y, z, 1);
        vec3 pointnormclip = vec3(pointclip)/pointclip.w;

        vec3 lightDir = normalize(-vec3(1,1,1));
        float intensity = .2 + max(dot(lightDir,normalize(pointnormclip-centernormclip)), 0.0);
        outColor = intensity*vec4(Color,1);
    }


}
);


*/







/*
const char* VS_SOURCE = GLSL(450, 
layout (location = 0) in vec3 in_vertex;
layout (location = 2) in vec3 color;
layout (location = 3) in float scale;
uniform mat4 model;
uniform mat4 proj;
uniform mat4 MVP;
uniform vec4 viewport;

out vec3 Color;
out mat4 VPMTInverse;
out mat4 VPInverse;
out vec3 centernormclip;

void main(){
  float R = scale;
  vec4 temp = vec4(in_vertex, 1.0);
  Color = color;
  gl_PointSize = scale;
  gl_Position =  MVP*temp;

  mat4 T = mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		in_vertex.x/R, in_vertex.y/R, in_vertex.z/R, 1.0/R);
  mat4 PMTt = transpose(MVP*T);


  vec4 r1 = PMTt[0];
  vec4 r2 = PMTt[1];
  vec4 r4 = PMTt[3];
  float r1Dr4T = dot(r1.xyz,r4.xyz)-r1.w*r4.w;
  float r1Dr1T = dot(r1.xyz,r1.xyz)-r1.w*r1.w;
  float r4Dr4T = dot(r4.xyz,r4.xyz)-r4.w*r4.w;
  float r2Dr2T = dot(r2.xyz,r2.xyz)-r2.w*r2.w;
  float r2Dr4T = dot(r2.xyz,r4.xyz)-r2.w*r4.w;

  gl_Position = vec4(-r1Dr4T, -r2Dr4T, gl_Position.z/gl_Position.w*(-r4Dr4T), -r4Dr4T);

  
  float discriminant_x = r1Dr4T*r1Dr4T-r4Dr4T*r1Dr1T;
  float discriminant_y = r2Dr4T*r2Dr4T-r4Dr4T*r2Dr2T;
  float screen = max(float(viewport.z), float(viewport.w));
  
  gl_PointSize = sqrt(max(discriminant_x, discriminant_y))*screen/(-r4Dr4T);



  mat4 TInverse = mat4(
       1.0,          0.0,          0.0,         0.0,
       0.0,          1.0,          0.0,         0.0,
       0.0,          0.0,          1.0,         0.0,
       -in_vertex.x, -in_vertex.y, -in_vertex.z, R);
  mat4 VInverse = mat4(
       2.0/float(viewport.z), 0.0, 0.0, 0.0,
       0.0, 2.0/float(viewport.w), 0.0, 0.0,
       0.0, 0.0,                   2.0/gl_DepthRange.diff, 0.0,
       -float(viewport.z+2.0*viewport.x)/float(viewport.z),
       -float(viewport.w+2.0*viewport.y)/float(viewport.w),
       -(gl_DepthRange.near+gl_DepthRange.far)/gl_DepthRange.diff, 1.0);

  VPMTInverse = TInverse*inverse(MVP)*VInverse;
  VPInverse = inverse(proj)*VInverse;
  vec4 centerclip = model*temp;
  centernormclip = vec3(centerclip)/centerclip.w;


}
);
*/



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


