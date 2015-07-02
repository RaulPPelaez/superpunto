#ifndef _RMODELHANDLER_H_
#define _RMODELHANDLER_H_ 

#include "glib.h"


class RSkyBox{
	public:
		RSkyBox(){}
		~RSkyBox(){
			fori(1,6)glDeleteTextures(1,tex[i].id());
		}
	
	private:
	std::vector<RTexture> tex;
	
};





class RModelHandler{
public:
   RModelHandler(){}
  ~RModelHandler(){
    fori(0,N_models){
      glDeleteBuffers(3, vbos[i]);
      glDeleteVertexArrays(1, &vaos[i]);
    }
    fori(0,textures.size()){
      glDeleteTextures(1,textures[i].id()); 
    }
  }
  void initialize(glm::mat4 *MVP, glm::mat4 *model);
  int add_model(const char* fileName);
  void set_model_texture(GLuint i, RTexture tex){ textures[i] = tex;}
  void set_instancing(const GLchar *attrib, GLuint i, GLuint vbo, GLuint N, GLuint size, GLuint stride);
  void set_attributes_instanced(GLuint i);
  
  void draw_model(GLuint i);
  
  RShaderProgram * get_pr(){ return &(this->pr);}
  
private:
  void config_light();
  void create_program();
  void set_attributes(GLuint *vbo);

  unsigned int N_models;
  std::vector<GLuint> vaos;
  std::vector<GLuint*> vbos;
  std::vector<GLuint> instancing_vbos;
  std::vector<GLuint> Ninstances;
  std::vector<GLuint> Nvertex;
  std::vector<RTexture> textures;
  //RTexture earth_texture;
  RShader vs, fs;
  RShaderProgram pr;
  GLuint unimodel, uniMVP;
  glm::mat4 *MVP, *model;
  
};

#endif