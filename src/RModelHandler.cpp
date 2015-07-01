#include "RModelHandler.h"

void RModelHandler::initialize(glm::mat4 *MVP, glm::mat4 *model){
  this-> N_models = 0;
  this-> MVP = MVP;
  this-> model = model;
  
  this->create_program();


  //this->add_model("sphere3.obj");
  
  //textures[0].Load("earth.png");
  glUniform1i(glGetUniformLocation(pr.id(), "tex"), 1);

  //textures.push_back(texture);
  
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
  //glGenBuffers(1, &positions_vbo);
  //glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);
  //glBufferData(GL_ARRAY_BUFFER, N*N*sizeof(glm::vec4), 0, GL_DYNAMIC_DRAW);
  
  //std::string inputfile = "sphere3.obj";
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string err = tinyobj::LoadObj(shapes, materials, fileName /*inputfile.c_str()*/);
  cout<<err<<endl;
  Nvertex[i_model] = shapes[0].mesh.indices.size();
  printf("New model loaded: %d vertex, %d index, %d normals, %d texcoord\n"
	,(int)shapes[0].mesh.positions.size()/3, (int)shapes[0].mesh.indices.size(),
	 (int)shapes[0].mesh.normals.size()/3,(int)shapes[0].mesh.texcoords.size()/2); 
  
  
  vbos.push_back(new GLuint[4]);
  glGenBuffers(4, vbos[i_model]);

  glBindBuffer(GL_ARRAY_BUFFER, vbos[i_model][0]);
  glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.positions.size()*sizeof(float), shapes[0].mesh.positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
    
  glBindBuffer(GL_ARRAY_BUFFER, vbos[i_model][1]);
  glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.normals.size()*sizeof(float), shapes[0].mesh.normals.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[i_model][2]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[0].mesh.indices.size()*sizeof(unsigned int), shapes[0].mesh.indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


  glBindBuffer(GL_ARRAY_BUFFER, vbos[i_model][3]);
  glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.texcoords.size()*sizeof(float), shapes[0].mesh.texcoords.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  

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

  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  pr.set_attrib("normal", 3, 3*sizeof(GLfloat), 0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
  pr.set_attrib("texcoord", 2, 2*sizeof(GLfloat), 0); 
}
void RModelHandler::set_attributes_instanced(GLuint i){
  glBindBuffer(GL_ARRAY_BUFFER, vbos[i][0]);
  pr.set_attrib_instanced("in_vertex", 3, 3*sizeof(GLfloat), 0);

  glBindBuffer(GL_ARRAY_BUFFER, vbos[i][1]);
  pr.set_attrib_instanced("normal", 3, 3*sizeof(GLfloat), 0);

  glBindBuffer(GL_ARRAY_BUFFER, vbos[i][3]);
  pr.set_attrib_instanced("texcoord", 2, 2*sizeof(GLfloat), 0); 
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
  //glUniform1i(glGetUniformLocation(pr.id(), "tex"), textures[i].sampler_id());

      
    //textures[i].Bind();
    //printf("%d\n",*textures[i].id());
    glBindVertexArray(vaos[i]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[i][2]);
    
    if(instancing_vbos[i]!=0){
      //glBindBuffer(GL_ARRAY_BUFFER, instancing_vbos[i]);
      glDrawElementsInstanced( GL_TRIANGLES, Nvertex[i], GL_UNSIGNED_INT, 0, Ninstances[i]);     
    }
    else glDrawElements( GL_TRIANGLES, Nvertex[i], GL_UNSIGNED_INT, 0);
  
    
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  //glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
  pr.unbind();//glUseProgram(0);

}