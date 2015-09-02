#ifndef _RMODELHANDLER_H_
#define _RMODELHANDLER_H_ 

#include "glib.h"


class RModelHandler{
public:
   RModelHandler(){}
  ~RModelHandler(){
    //fori(0,N_models){
      glDeleteBuffers(2, vbos);
      glDeleteBuffers(1, &line_vbo);
      glDeleteVertexArrays(1, &line_vao);
      glDeleteVertexArrays(1, &vao);
      //}
    //fori(0,textures.size()){
    //  glDeleteTextures(1,textures[i].id()); 
    //}
  }
  void initialize(glm::mat4 *MVP, glm::mat4 *model);
  //void set_model_texture(GLuint i, RTexture tex){ textures[i] = tex;}
  void set_instancing(const GLchar *attrib, GLuint vbo, GLuint N, GLuint size, GLuint stride);
  //void set_attributes_instanced();
  
  void draw_model();
  void draw_lines();
  RShaderProgram * get_pr(){ return &(this->pr);}
  void config_light();
  GLuint line_vbo;
  GLuint Nlines;
private:
  void create_program();
  //void set_attributes(GLuint *vbo);
  int add_models();

  unsigned int N_models;
  GLuint vao, line_vao;
  GLuint* vbos;
  GLuint instancing_vbo;
  GLuint Ninstances;
  GLuint Nvertex;
  //std::vector<RTexture> textures;
  //RTexture earth_texture;

  RShaderProgram pr, line_pr;
  GLuint unimodel, uniMVP, uniMVP_line;
  glm::mat4 *MVP, *model;
  
};

#endif
