#include "RRenderer.h"


RRenderer::RRenderer(RConfig i_cfg, float gscale):cam(),
						  gscale(gscale),
						  cfg(i_cfg),
						  box(gscale, &MVP),
						  axis(&MVP,&(cam.pos), glm::vec3(-10,0,0) , cfg){
  picked[0] = picked[1] = -1;
  textRenderer.setFont(cfg.fontName.c_str(), 45);

  proj = glm::perspective(FOV, FWIDTH/(float)FHEIGHT, ZNEAR, ZFAR);
  model = glm::mat4();
  view = cam.lookAt();
  rotate_model(M_PI/4.0f, 1.0f, 0.0f, 0.0f);

  MVP = proj*view*model;

  
}



void RRenderer::handle_resize(){
  glViewport(0,0,FWIDTH, FHEIGHT);
  float ar = FWIDTH/(float)FHEIGHT;
  proj =glm::perspective(FOV, ar, ZNEAR, ZFAR);

  //fbo.handle_resize();
}


void RRenderer::rotate_model(GLfloat angle, GLfloat x, GLfloat y, GLfloat z){
  model = glm::rotate(model, angle,  glm::vec3(x, y, z));
}


void RRenderer::update(){
  view = cam.lookAt();
  MVP = proj*view*model;

  
  cam.update();
}

void RRenderer::handle_event(SDL_Event &e){

}

void RRenderer::display(){


  
  glEnable(GL_BLEND);
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  textRenderer.draw();
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);

  if(!cfg.noaxis) axis.draw();

}

void RRenderer::drawText(const char* text, int x, int y){
  textRenderer.setText(text, x,y);
}



int RRenderer::pick(int x, int y, int pickindex){
  return -1;
}

Uint8* RRenderer::getPixels(){
  static vector<Uint8> cdata;
  int cdatasize = FWIDTH*FHEIGHT*4;  
  if(cdata.size() != cdatasize) cdata.resize(cdatasize);

  glReadPixels(0,0, FWIDTH,FHEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, (void *)cdata.data());
  return cdata.data();
}

glm::int2 RRenderer::getSize(){
  return glm::int2(FWIDTH, FHEIGHT);
}

#include"shaders.h"


RAxis::RAxis(glm::mat4 *MVP, glm::vec3 *campos, glm::vec3 origin, RConfig cfg):
  MVP(MVP),
  campos(campos),
  origin(origin),
  axislength(5.0f){
  Xtext.setFont(cfg.fontName.c_str(), 45);
  Ytext.setFont(cfg.fontName.c_str(), 45);
  Ztext.setFont(cfg.fontName.c_str(), 45);
  
  Xtext.setText("X",0,0);
  Ytext.setText("Y",0,0);
  Ztext.setText("Z",0,0);

  RShader shs[3];
  shs[0].charload(shaders_axis_vs, GL_VERTEX_SHADER);
  shs[1].charload(shaders_axis_gs, GL_GEOMETRY_SHADER);
  shs[2].charload(shaders_axis_fs, GL_FRAGMENT_SHADER);
  pr.init(shs, 3);
}


void RAxis::draw(){

  
  
  glm::vec4 xpos = *MVP*(glm::vec4(origin,0)+glm::vec4(axislength,0,0,1));
  glm::vec4 ypos = *MVP*(glm::vec4(origin,0)+glm::vec4(0,axislength,0,1));
  glm::vec4 zpos = *MVP*(glm::vec4(origin,0)+glm::vec4(0,0,axislength,1));

  
  xpos.x /= xpos.w; ypos.x/=ypos.w; zpos.x/=zpos.w;
  xpos.y /=xpos.w; ypos.y/=ypos.w; zpos.y/=zpos.w;

  
  float dist = glm::length(*campos);
  
  Xtext.setText("X", 0.5f*(xpos.x+1)*FWIDTH, FHEIGHT-0.5f*(xpos.y+1)*FHEIGHT, 35.f/dist);
  Ytext.setText("Y", 0.5f*(ypos.x+1)*FWIDTH, FHEIGHT-0.5f*(ypos.y+1)*FHEIGHT, 35.f/dist);
  Ztext.setText("Z", 0.5f*(zpos.x+1)*FWIDTH, FHEIGHT-0.5f*(zpos.y+1)*FHEIGHT, 35.f/dist);

  glEnable(GL_DEPTH_TEST);
  pr.use();

  glUniformMatrix4fv(glGetUniformLocation(pr.id(), "MVP")
		     , 1, GL_FALSE, glm::value_ptr(*MVP));
  glUniform1f(glGetUniformLocation(pr.id(), "axislength"), 5.0f);
  glUniform3f(glGetUniformLocation(pr.id(), "origin"), origin.x, origin.y, origin.z);

  glLineWidth(5.0f);
  dummy_vao.use();
  glDrawArrays(GL_POINTS, 0, 3);
  dummy_vao.unbind();
  pr.unbind();
  glLineWidth(2.0f);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  Xtext.draw();
  Ytext.draw();
  Ztext.draw();
  
  glDisable(GL_BLEND);
  

}

