#include "RGL.h"


void RGL::initialize(int options){
  
  frames = 0.0;

  drawables.initialize(&MVP, &model, options);

  MVP = glm::mat4();
  model = glm::mat4();

  initBuffers();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_MULTISAMPLE);
  proj =glm::perspective(45.0f, 1.0f, 0.01f, 10000.0f);

  lightpos = glm::vec3(0,0,-10);
  loadIdentity();
  printf("DONE!\nDrawing...\n");
  
}



void RGL::setUniform(const char* name, float val){
 drawables.get_pr()->use();

  glUniform1f(glGetUniformLocation(drawables.get_pr()->id(), name), val);

  drawables.get_pr()->unbind();
}

void RGL::draw(){
  //  drawables.compute_shadows();
  if(post_processor.isEnabled())post_processor.bind();
  
  glEnable(GL_DEPTH_TEST);  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  this->view = cam.lookAt();
  drawables.get_pr()->use();

  glUniform3f(glGetUniformLocation(drawables.get_pr()->id(), "EyeWorldPos"), cam.pos.x, cam.pos.y, cam.pos.z);
  glUniform3f(glGetUniformLocation(drawables.get_pr()->id(),"point_light.Position"), lightpos.x, lightpos.y, lightpos.z);

  upload_MVP();
  drawables.draw_model();


}

void RGL::post_process(){
  if(post_processor.isEnabled())post_processor.post_process();
}


void RGL::update(){this->cam.update();}

void RGL::handle_event(sf::Event event){
  if (event.type == Event::KeyPressed){
    IF_KEY(Up, lightpos.z += 0.5;)
      IF_KEY(Down, lightpos.z -= 0.5;)
      IF_KEY(Space, upload_step();)

      //IF_KEY(R, current_step= Rmax(current_step-2,0);upload_step();)

      //      IF_KEY(M, play_movie = !play_movie;)

      IF_KEY(Num4, rotate(0.1f,1,0,0);)
      IF_KEY(Num5, rotate(0.1f,0,1,0);)
      IF_KEY(Num6, rotate(0.1f,0,0,1);)

      IF_KEY(Num1, rotate(-0.1f,1,0,0);)
      IF_KEY(Num2, rotate(-0.1f,0,1,0);)
      IF_KEY(Num3, rotate(-0.1f,0,0,1);)
      }
}


void RGL::rotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z){
  model = glm::rotate(model, angle,  glm::vec3(x, y, z));
}
void RGL::scale(GLfloat x, GLfloat y, GLfloat z){
  model = glm::scale(model, glm::vec3(x, y, z));
}
void RGL::translate(GLfloat x, GLfloat y, GLfloat z){
  model = glm::translate(model, glm::vec3(x, y, z));
}
void RGL::lookAt(const glm::vec3 &pos, const glm::vec3 &view, const glm::vec3 &up){
  this->view = glm::lookAt(pos,view,up);
}

void RGL::loadIdentity(){model = glm::mat4();}

void RGL::upload_MVP(){MVP = proj*view*model;}

void RGL::change_aspect(float new_aspect){
  this->proj =glm::perspective(45.0f, new_aspect, 0.01f, 10000.0f);
}

