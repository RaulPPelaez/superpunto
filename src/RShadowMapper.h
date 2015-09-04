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
  RShadowMapper(){}
  void init();



 private:
  GLuint fb, stex;
  RShaderProgram pr;
  GLuint MVP, DepthBias, ShadowMapID;
  glm::mat4 depthMVP;
  GLuint depthMVPid;
};



#endif
