#include "RModelHandler.h"

void RModelHandler::initialize(glm::mat4 *MVP, glm::mat4 *model){
  this-> N_models = 0;
  this-> MVP = MVP;
  this-> model = model;
  
  this->create_program();

  unimodel = glGetUniformLocation(pr.id(), "model");
  uniMVP = glGetUniformLocation(pr.id(), "MVP");

  this->config_light();
  glUseProgram(0);
  
}

int RModelHandler::add_model(const char* fileName){
  if(!pr.is_in_use()) pr.use();
  this->N_models++;
  unsigned int i_model = N_models-1;
  textures.resize(N_models);
  instancing_vbos.resize(N_models,0);
  Ninstances.resize(N_models,0);
  Nvertex.resize(N_models);

  
  vbos.push_back(new GLuint[2]);

  generate_sphere_vbos(vbos[i_model][0],vbos[i_model][1]);
  Nvertex[i_model] = 240; //Number of indexes of the sphere.

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  this->set_attributes(vbos[i_model]);
  glBindVertexArray(0);
  
  vaos.push_back(vao);
    
  return i_model;

}

void RModelHandler::create_program(){
  vs.load("../res/vertex_shader.glsl",GL_VERTEX_SHADER);
  fs.load("../res/fragment_shader.glsl", GL_FRAGMENT_SHADER);

  pr.create();
  pr.add_shader(&vs);
  pr.add_shader(&fs);
  pr.link();
  pr.use(); 
  
}

void RModelHandler::set_attributes(GLuint *vbo){
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  pr.set_attrib("in_vertex", 3, 3*sizeof(GLfloat), 0);
}
void RModelHandler::set_attributes_instanced(GLuint i){
  glBindBuffer(GL_ARRAY_BUFFER, vbos[i][0]);
  pr.set_attrib_instanced("in_vertex", 3, 3*sizeof(GLfloat), 0);
}

void RModelHandler::set_instancing(const GLchar *attrib, GLuint i, GLuint vbo, GLuint N, GLuint size, GLuint stride){
  instancing_vbos[i] = vbo;
  Ninstances[i] = N;
  pr.use();
  glBindVertexArray(vaos[i]);
  glBindBuffer(GL_ARRAY_BUFFER,vbo);
  pr.set_attrib_instanced(attrib, size, stride,0);
  glBindVertexArray(0);

  
}
  
void RModelHandler::config_light(){
  /*
  glUniform3f(glGetUniformLocation(pr.id(), "light.Color"), 1, 1, 1);
  glUniform1f(glGetUniformLocation(pr.id(),"light.Ambient"), 0.2f);
  glm::vec3 Direction = glm::normalize(glm::vec3(-1,1,1));
  glUniform3f(glGetUniformLocation(pr.id(),"Directional_light"), Direction.x, Direction.y, Direction.z);
  glUniform1f(glGetUniformLocation(pr.id(),"light.Diffuse"), 1.0f);
  */

  glUniform1f(glGetUniformLocation(pr.id(),"MatSpecularIntensity"), 1.1f);
  glUniform1i(glGetUniformLocation(pr.id(),"SpecularPower"), 32);

  glUniform3f(glGetUniformLocation(pr.id(),"point_light.Position"), 0, 0, 0);

  glUniform1f(glGetUniformLocation(pr.id(),"point_light.Atten.Constant"), 1.0f);
  glUniform1f(glGetUniformLocation(pr.id(),"point_light.Atten.Linear"), 0.000000001f);
  glUniform1f(glGetUniformLocation(pr.id(),"point_light.Atten.Exp"), 0.00000000001f);

  glUniform3f(glGetUniformLocation(pr.id(),"point_light.Base.Color"), 1,1,1);
  glUniform1f(glGetUniformLocation(pr.id(),"point_light.Base.Diffuse"), 1.0f);
  glUniform1f(glGetUniformLocation(pr.id(),"point_light.Base.Ambient"), 0.25f);
}
  
  
  
  
void RModelHandler::draw_model(GLuint i){
  if(i>=N_models) return;
  pr.use();
  
  glUniformMatrix4fv(uniMVP , 1, GL_FALSE, glm::value_ptr(*MVP) );
  glUniformMatrix4fv(unimodel , 1, GL_FALSE, glm::value_ptr(*model));

    glBindVertexArray(vaos[i]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[i][1]);
    
    if(instancing_vbos[i]!=0){
      glDrawElementsInstanced( GL_TRIANGLES, Nvertex[i], GL_UNSIGNED_INT, 0, Ninstances[i]);     
    }
    else glDrawElements( GL_TRIANGLES, Nvertex[i], GL_UNSIGNED_INT, 0);
  
    
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  pr.unbind();//glUseProgram(0);

}