#ifndef RSHADOWMAPPER_H
#define RSHADOWMAPPER_H

#include <defines.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include<glib.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



class RShadowMapper{
 public:
  RShadowMapper(){enabled = false;}
  void init();
  void bind(){pr.use();}
  void attach_shadowmap(GLuint prr);
  void prepare_to_draw(glm::mat4 model);
  void flush();

  bool isEnabled(){return this->enabled;}

 private:
  GLuint fb, stex;
  RShaderProgram pr;
  GLuint MVP, DepthBias, ShadowMapID;
  glm::mat4 model_matrix, depthP, depthV, depthMVP, depthBiasPV;
  GLuint depthMVPid;

  bool enabled;
};



#endif
