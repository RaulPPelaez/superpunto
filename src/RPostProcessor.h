#ifndef RPSTPROCESSOR_H
#define POSTPROCESSOR_H
#include<defines.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include<glib.h>


class RPostProcessor{
 public:
  RPostProcessor(){enabled = false;}
  ~RPostProcessor(){
    glDeleteFramebuffers(1, &fb);
    glDeleteRenderbuffers(1, &rb);
    glDeleteVertexArrays(1, &pp_vao);
    glDeleteBuffers(1, &pp_vbo);
    glDeleteTextures(1, &pp_tex);
  }
  void init();
  void bind(){
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
  }
  void post_process();
  bool isEnabled(){ return this->enabled; }
 private:
  GLuint fb, rb; //FrameBuffer and RenderBuffer
  //RShader sc_vs, sc_fs; //screen vs and fs
  GLuint pp_vao, pp_vbo, pp_tex;
  RShaderProgram pp_pr; //Program for post_procesing 
  

  bool enabled;
};

#endif
