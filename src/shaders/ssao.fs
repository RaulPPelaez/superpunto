#version 450


// uniform sampler2D ctex; //Color tex
// uniform sampler2D dtex; //depth buffer
 uniform sampler2D ndtex; //Normal and linear depth tex
// uniform sampler2D ptex;  //Position buffer
 uniform sampler2D noisetex; //4x4 noise texture

 out float occ;


const int NSAMPLES = 129;
uniform vec4 points[NSAMPLES];
/*This radius works well for radius 1 spheres*/
uniform float radius = 0.015f;

void main(){
  occ = 0.0;
  vec4 ND = texelFetch(ndtex, ivec2(gl_FragCoord.xy), 0);
  
  vec3 N = ND.xyz;
  float my_depth = ND.w;

  /*If this is a background pixel*/
  if(my_depth==1.0)  return;

  /*Create a random rotated base to randomize directions*/
  vec3 rv = texture(noisetex,
		    ivec2(gl_FragCoord.xy/800.0/4.0)).xyz;
  
  vec3 tangent = normalize(rv - N*dot(rv, N));
  vec3 bitan = cross(N, tangent);
  mat3 TBN = mat3(tangent, bitan, N);

  float z = my_depth;
  for(int i = 0; i<NSAMPLES; i++){
    /*Rotate the direction*/
    vec3 dir = TBN*points[i].xyz;
    /*The sample point is at dir*radius from this point*/
    vec3 sm = vec3(gl_FragCoord.xy/800.0, z) + dir*radius;
    /*Mirror the points in the hidden hemisphere*/
    if(dot(N, dir) <0.0) dir = -dir;
    /*Recover the depth of the sample point*/
    float their_depth =
      textureLod(ndtex,
		 sm.xy
		 , 0).w;
    /*depth is 1.0f in the background,
      this prevents the background from occluding the model*/
    if(their_depth!=1.0){
      float delta = (z-their_depth);
      /*Occlude the pixel if the sample is closer*/
      /*This number works well for radius one spheres*/
      if( delta>3.0005e-4 ) {occ += 2.0;}
    }

  }
  
  occ = (1.0 - occ/NSAMPLES);
  if(occ<0.0) occ=0.0;

 
}







// c = texelFetch(ctex, ivec2(gl_FragCoord.xy), 0);
// c.w = 1;
