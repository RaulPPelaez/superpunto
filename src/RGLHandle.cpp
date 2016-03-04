#include "RGLHandle.h"

RGLHandle::RGLHandle(){}
RGLHandle::~RGLHandle(){}
                                                                                               

bool RGLHandle::init(int maxN){
  printf("Initializing OpenGL...\n");
  this->handle_resize();

  this->maxN = maxN;
  attribs["in_vertex"] = 0;
  attribs["color"] = 2;
  attribs["scale"] = 3;

  init_buffers();
  init_math();
  init_shaders();
  init_uniforms();
  printf("DONE!\n");
  return true;
}

bool RGLHandle::init_buffers(){
  printf("\tInit buffers...     ");
  init_instance_vbos();
  init_vao();
  printf("DONE!\n");
  return true;
}

bool RGLHandle::init_vao(){ //Configure Vertex Array Object
  spheres_vao.set_attrib(attribs["in_vertex"], instances_vbos[0], 0);
  spheres_vao.set_attrib(attribs["color"    ], instances_vbos[1], 2);
  spheres_vao.set_attrib(attribs["scale"    ], instances_vbos[2], 3);

  return true;
}

bool RGLHandle::init_instance_vbos(){
  DataLayout dl;
  dl.init(3, 3*sizeof(float), GL_FLOAT, 0, 0);
  instances_vbos[0].init(GL_ARRAY_BUFFER, GL_DYNAMIC_STORAGE_BIT, dl);
  instances_vbos[1].init(GL_ARRAY_BUFFER, GL_DYNAMIC_STORAGE_BIT, dl);
  dl.init(1, sizeof(float), GL_FLOAT, 0, 0);
  instances_vbos[2].init(GL_ARRAY_BUFFER, GL_DYNAMIC_STORAGE_BIT, dl);
  //Create buffer with max possible N
  instances_vbos[0].initmem(maxN*sizeof(float)*3, NULL);  //poss
  instances_vbos[1].initmem(maxN*sizeof(float)*3, NULL);  //colors
  instances_vbos[2].initmem(maxN*sizeof(float)*1, NULL);  //scales
  return true;
}


bool RGLHandle::init_math(){
  float ratio = FWIDTH/(float)FHEIGHT;
  //proj =glm::perspective(45.0f, ratio, 1.0f, 100.0f);
  proj =glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 10000.0f);   
  model = glm::mat4();
  view = cam.lookAt();
  rotate_model(M_PI/4.0f, 1.0f, 0.0f, 0.0f);

  MVP = proj*view*model;

  return true;
}

#include "shaders.h"
bool RGLHandle::init_shaders(){  
  printf("\tInit shaders...     ");
  RShader shs[2];
  shs[0].charload(VS_SOURCE, GL_VERTEX_SHADER);
  shs[1].charload(FS_SOURCE, GL_FRAGMENT_SHADER);
  pr.init(shs, 2);
  printf("DONE!\n");
  
  return true;
}
bool RGLHandle::init_uniforms(){
  printf("\tInit uniforms...    ");
  pr.use();
  uniforms["MVP"]      = glGetUniformLocation(pr.id(), "MVP"     );
  uniforms["model"]    = glGetUniformLocation(pr.id(), "model"   );
  uniforms["viewport"] = glGetUniformLocation(pr.id(), "viewport");
  uniforms["proj"]     = glGetUniformLocation(pr.id(), "proj");
  
  pr.unbind();
  printf("DONE!\n");
  return true;
}


bool RGLHandle::upload_instances(const float *pos, const float *colors, const float *scales, int N){  
  instances_vbos[0].upload(0, 3*N*sizeof(float), (const void *)pos);
  instances_vbos[1].upload(0, 3*N*sizeof(float), (const void *)colors);
  instances_vbos[2].upload(0, 1*N*sizeof(float), (const void *)scales);
  currentN = N;
  return true;
}

void RGLHandle::handle_resize(){
  glViewport(0,0,FWIDTH, FHEIGHT);
  float ar = FWIDTH/(float)FHEIGHT;
  //proj = glm::perspective(45.0f, ar, 0.01f, 10000.0f);
  proj =glm::ortho(-10.0f, 10.0f, -10.0f/ar, 10.0f/ar, -1.0f, 10000.0f);   
}

void RGLHandle::rotate_model(GLfloat angle, GLfloat x, GLfloat y, GLfloat z){
  model = glm::rotate(model, angle,  glm::vec3(x, y, z));
}

void RGLHandle::update(){
  cam.update();
}

void RGLHandle::draw(){
  pr.use();
  view = cam.lookAt();
  MVP = proj*view*model;
  glUniformMatrix4fv(uniforms["MVP"]   , 1, GL_FALSE, glm::value_ptr(MVP));
  glUniformMatrix4fv(uniforms["model"] , 1, GL_FALSE, glm::value_ptr(view*model));
  glUniformMatrix4fv(uniforms["proj"]  , 1, GL_FALSE, glm::value_ptr(proj));

  GLfloat m_viewport[4];glGetFloatv(GL_VIEWPORT, m_viewport);
  glUniform4fv(uniforms["viewport"], 1, m_viewport);

  spheres_vao.use();
  glDrawArrays(GL_POINTS, 0, currentN);
  spheres_vao.unbind();
  pr.unbind();
}
