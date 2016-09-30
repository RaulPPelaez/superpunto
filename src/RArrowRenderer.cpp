#include"RArrowRenderer.h"


RArrowRenderer::RArrowRenderer(int maxN, float gscale, RConfig cfg):
  RRenderer(cfg, gscale),
  maxN(maxN),
  dirscale(1.0f){

  cerr<<"Initializing Arrow Renderer..."<<endl;  
  this->handle_resize();

  attribs["pos"] = 0;
  attribs["dir"] = 1;
  attribs["color"] = 2;

  init_buffers();
  init_shaders();
  init_uniforms();
  
  cerr<<"DONE!"<<endl;
}

bool RArrowRenderer::init_buffers(){
  cerr<<"\tInit buffers...     ";
  DataLayout dl;
  dl.init(3, 3*sizeof(float), GL_FLOAT, 0);
  fori(0,3){
    arrow_vbos[i].init(GL_ARRAY_BUFFER, GL_DYNAMIC_STORAGE_BIT, dl);
    arrow_vbos[i].initmem(maxN*sizeof(float)*3, NULL);
  }

  vao.set_attrib(attribs["pos"], arrow_vbos[0], 0);
  vao.set_attrib(attribs["dir"], arrow_vbos[1], 1);
  vao.set_attrib(attribs["color"], arrow_vbos[2], 2);
  
  cerr<<"DONE!"<<endl;
  return true;
}

#include"shaders.h"
bool RArrowRenderer::init_shaders(){
  cerr<<"\tInit Shaders...     ";
  RShader shs[3];
  
  shs[0].charload(shaders_arrow_vs, GL_VERTEX_SHADER);
  shs[1].charload(shaders_arrow_fs, GL_FRAGMENT_SHADER);
  shs[2].charload(shaders_arrow_gs, GL_GEOMETRY_SHADER);
  pr.init(shs, 3);
  
  cerr<<"DONE!"<<endl;
  return true;
}

bool RArrowRenderer::init_uniforms(){
  cerr<<"\tInit Uniforms...     ";
  pr.use();
  uniMVP = glGetUniformLocation(pr.id(), "MVP");
  uniproj = glGetUniformLocation(pr.id(), "proj");
  unimodel = glGetUniformLocation(pr.id(), "model");
  unicam = glGetUniformLocation(pr.id(), "campos");
  uniview = glGetUniformLocation(pr.id(), "view");

  glUniform1f(glGetUniformLocation(pr.id(), "gscale"),
	      this->gscale);
  pr.unbind();

  cerr<<"DONE!"<<endl;
  return true;
}


void RArrowRenderer::handle_event(SDL_Event &e){
 
  if(e.type == SDL_KEYDOWN){
    IF_KEY(j,  dirscale *= 1.1;)
    IF_KEY(k,  dirscale /= 1.1;)
  }
  pr.use();
  int unidir =  glGetUniformLocation(pr.id(), "dirscale");
  glUniform1f(unidir, dirscale);

  pr.unbind();

}

void RArrowRenderer::update(){

  RRenderer::update();
}


void RArrowRenderer::draw(){
  //fbo.use();
  glLineWidth(0.5f*pow(dirscale,0.4));
  glEnable(GL_DEPTH_TEST);
  glClearColor(cfg.bcolor[0], cfg.bcolor[1], cfg.bcolor[2], 1.0f);  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  pr.use();
  glUniformMatrix4fv(uniMVP , 1, GL_FALSE, glm::value_ptr(MVP));
  glUniformMatrix4fv(uniproj , 1, GL_FALSE, glm::value_ptr(proj));
  glUniformMatrix4fv(unimodel , 1, GL_FALSE, glm::value_ptr(model));
  
  glUniformMatrix4fv(uniview , 1, GL_FALSE, glm::value_ptr(view));
  glm::vec3 campos = cam.pos;
  glUniform3fv(unicam , 1, glm::value_ptr(campos));

  vao.use();
  glDrawArrays(GL_POINTS, 0, particles.N);
  vao.unbind();
  pr.unbind();

  box.draw();
  RRenderer::display();
}

bool RArrowRenderer::upload_instances(ParticleData pdata){
  int N = pdata.N;
  if(pdata.vel == nullptr){
    cerr<<"Not arrow data available in file!!"<<endl;
    exit(1);
  }
  arrow_vbos[0].upload(0, 3*N*sizeof(float),
		       (const void *)pdata.pos);
  arrow_vbos[2].upload(0, 3*N*sizeof(float),
		       (const void *)pdata.colors);
  arrow_vbos[1].upload(0, 3*N*sizeof(float),
		       (const void *)pdata.vel);
  this->particles = pdata;

  float3 L = pdata.L;
  box.setSize(glm::vec3(L.x, L.y, L.z));
  
  return true;
}
