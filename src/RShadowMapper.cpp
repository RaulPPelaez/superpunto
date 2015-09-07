#include"RShadowMapper.h"


void RShadowMapper::init(){
  glGenFramebuffers(1, &fb);
  glBindFramebuffer(GL_FRAMEBUFFER, fb);

  glGenTextures(1, &stex);
  glBindTexture(GL_TEXTURE_2D, stex);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, SHADOWMAP_X, SHADOWMAP_Y, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
 
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, stex, 0);
 
  glDrawBuffer(GL_NONE); // No color buffer is drawn to


  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  RShader vs, fs;
  const char *VS_SOURCE = GLSL(330,
			       layout (location = 0) in vec3 in_vertex; //vertex position
			       layout (location = 1) in vec3 pos; //Instance position
			       layout (location = 2) in vec3 color; //Instanced color 
			       layout (location = 3) in float scale; //Instanced scale
			       uniform mat4 depthMVP;
			       void main () {
				 gl_Position = depthMVP*(vec4(scale*in_vertex.xyz+pos.xyz,1.0));
			       }
			       
			       );
  const char *FS_SOURCE = GLSL(330,
			       layout(location = 0) out float fragmentdepth;
			       void main(){
				 fragmentdepth = gl_FragCoord.z;
			       }
			       );

  vs.charload(VS_SOURCE, GL_VERTEX_SHADER);
  fs.charload(FS_SOURCE, GL_FRAGMENT_SHADER);
  pr.initialize(vs, fs);


  glm::vec3 lightInvDir = glm::vec3(1,1,1);

  // Compute the MVP matrix from the light's point of view
  glm::mat4 depthProjectionMatrix = glm::ortho<float>(-100,100,-100,100,-100,200);
  glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0,0,0), glm::vec3(0,1,0));
  glm::mat4 depthModelMatrix = glm::mat4(1.0);
  depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

  glm::mat4 biasMatrix(
		       0.5, 0.0, 0.0, 0.0,
		       0.0, 0.5, 0.0, 0.0,
		       0.0, 0.0, 0.5, 0.0,
		       0.5, 0.5, 0.5, 1.0
		       );
  depthBiasMVP = biasMatrix*depthMVP;

  depthMVPid = pr.get_attrib_handle("depthMVP");
  
  glUniformMatrix4fv(depthMVPid, 1, GL_FALSE, &depthMVP[0][0]);

  this->enabled = true;
}


void RShadowMapper::prepare_to_draw(){

  glBindFramebuffer(GL_FRAMEBUFFER, fb);
  glViewport(0,0,SHADOWMAP_X,SHADOWMAP_Y);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  pr.use();

}

void RShadowMapper::flush(){

  pr.unbind();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RShadowMapper::attach_shadowmap(GLuint prr){
  glUniformMatrix4fv(glGetUniformLocation(prr,"depthBiasMVP"), 1, GL_FALSE, &depthBiasMVP[0][0]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, stex);
  glUniform1i(glGetUniformLocation(prr,"shadowmap"), 0);

}
