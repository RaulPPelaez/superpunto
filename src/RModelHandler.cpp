#include "RModelHandler.h"


void RModelHandler::initialize(glm::mat4 *MVP, glm::mat4 *model, int options){
  this-> N_models = 0;
  this-> MVP = MVP;
  this-> model = model;
  this->create_program();
  unimodel = glGetUniformLocation(pr.id(), "model");
  uniMVP = glGetUniformLocation(pr.id(), "MVP");
  glUniform1f(glGetUniformLocation(pr.id(),"gscale"), 1.0f); 
  line_pr.use();
  uniMVP_line = glGetUniformLocation(line_pr.id(), "MVP");
  line_pr.unbind();
  this->add_models();
  
  if(options & RGL_SHADOWMAPS) shadow_processor.init();
}

int RModelHandler::add_models(){
   pr.use();
  //textures.resize(N_models);

  vbos = new GLuint[2];

  generate_sphere_vbos(vbos[0],vbos[1]);
  Nvertex = 240; //Number of indexes of the sphere.

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
  pr.set_attrib("in_vertex", 3, 3*sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
   
  line_pr.use();
  glGenBuffers(1, &line_vbo);
  glGenVertexArrays(1, &line_vao);
  glBindVertexArray(line_vao);
  glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
  glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
  line_pr.set_attrib("in_vertex", 3, 3*sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  line_pr.unbind();
  
  return 0;

}

void RModelHandler::create_program(){
  RShader vs, fs, line_vs, line_fs;
  const char* VS_SOURCE = GLSL(130,                                                        
			       in vec3 in_vertex;
			       uniform mat4 MVP;
			       void main() {                                               
				 gl_Position = MVP*(vec4(in_vertex.xyz,1.0));
			       }        
			       );
  
  const char* FS_SOURCE = GLSL(130,                                                        
			       out vec4 color;
			       void main() {                                               
				 color = vec4(1,0,0,1);
			       }        
			       );
  line_vs.charload(VS_SOURCE, GL_VERTEX_SHADER);
  line_fs.charload(FS_SOURCE, GL_FRAGMENT_SHADER);
  //delete[] VS_SOURCE;
  //  delete[] FS_SOURCE;
  line_pr.create();
  line_pr.add_shader(line_vs);
  line_pr.add_shader(line_fs);
  line_pr.link();
  line_pr.use();
  line_pr.unbind();
  vs.load("../res/vertex_shader.glsl",GL_VERTEX_SHADER);
  fs.load("../res/fragment_shader.glsl", GL_FRAGMENT_SHADER);
  //tcs.load("../res/tcs.glsl", GL_TESS_CONTROL_SHADER);
  //tes.load("../res/tes.glsl", GL_TESS_EVALUATION_SHADER);

  pr.create();
  pr.add_shader(vs);
  pr.add_shader(fs);
  pr.link();
  pr.use(); 
  
}

void RModelHandler::set_instancing(const GLchar *attrib, GLuint vbo, GLuint N, GLuint size, GLuint stride){
  instancing_vbo = vbo;
  Ninstances = N;
  pr.use();
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER,vbo);
  pr.set_attrib_instanced(attrib, size, stride,0);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);
  pr.unbind();
}
  
void RModelHandler::config_light(){
  pr.use();
  glUniform1f(glGetUniformLocation(pr.id(),"MatSpecularIntensity"), 1.1f);
  glUniform1i(glGetUniformLocation(pr.id(),"SpecularPower"), 32);

  glUniform1f(glGetUniformLocation(pr.id(),"point_light.Atten.Constant"), 1.0f);
  glUniform1f(glGetUniformLocation(pr.id(),"point_light.Atten.Linear"), 0.000000001f);
  glUniform1f(glGetUniformLocation(pr.id(),"point_light.Atten.Exp"), 0.00000000001f);

  glUniform3f(glGetUniformLocation(pr.id(),"point_light.Base.Color"), 1,1,1);
  glUniform1f(glGetUniformLocation(pr.id(),"point_light.Base.Diffuse"), 1.0f);
  glUniform1f(glGetUniformLocation(pr.id(),"point_light.Base.Ambient"), 0.25f);
  pr.unbind();
}
  
void RModelHandler::compute_shadows(){
  if(!shadow_processor.isEnabled())return;
  shadow_processor.prepare_to_draw();
  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);

  glDrawElementsInstanced( GL_TRIANGLES, Nvertex, GL_UNSIGNED_INT, 0, Ninstances);     
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  shadow_processor.flush();
}  
  
  
void RModelHandler::draw_model(){

  pr.use();

  if(shadow_processor.isEnabled()) shadow_processor.attach_shadowmap(pr.id());
  glUniformMatrix4fv(uniMVP , 1, GL_FALSE, glm::value_ptr(*MVP) );
  glUniformMatrix4fv(unimodel , 1, GL_FALSE, glm::value_ptr(*model));

  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
  glDrawElementsInstanced( GL_TRIANGLES, Nvertex, GL_UNSIGNED_INT, 0, Ninstances);     

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  pr.unbind();

}

void RModelHandler::draw_lines(){
  line_pr.use();
  glUniformMatrix4fv(uniMVP_line, 1, GL_FALSE, glm::value_ptr(*MVP) );
  glBindVertexArray(line_vao);
  //glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
  
  glDrawArrays(GL_LINES, 0, 72);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  line_pr.unbind();
  

}
