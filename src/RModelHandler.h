#ifndef _RMODELHANDLER_H_
#define _RMODELHANDLER_H_ 

#include "glib.h"



class RModelHandler{
public:
   RModelHandler(){}
  ~RModelHandler(){
      glDeleteBuffers(2, vbos);
      glDeleteBuffers(1, &line_vbo);
      glDeleteVertexArrays(1, &line_vao);
      glDeleteVertexArrays(1, &vao);
  }
  void initialize(glm::mat4 *MVP, glm::mat4 *model, int options);
  void set_instancing(const GLchar *attrib, GLuint vbo, GLuint N, GLuint size, GLuint stride);
    
  void draw_model();
  void draw_lines();
  RShaderProgram * get_pr(){ return &(this->pr);}
  void config_light();
  //void compute_shadows();
  GLuint line_vbo;
  GLuint Nlines;
private:
  void create_program();
  int add_models();

  unsigned int N_models;
  GLuint vao, line_vao;
  GLuint* vbos;
  GLuint instancing_vbo;
  GLuint Ninstances;
  GLuint Nvertex;

  //  RShadowMapper shadow_processor;

  RShaderProgram pr, line_pr;
  GLuint unimodel, uniMVP, uniMVP_line;
  glm::mat4 *MVP, *model;
  
};

#endif
