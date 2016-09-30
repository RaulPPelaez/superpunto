#ifndef RRENDERER_H
#define RRENDERER_H

#include"RFile.h"
#include"RGL.h"
#include"RTextRenderer.h"
#include"RBox.h"
#include"Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"RWindow.h"

#define FOV glm::radians(45.0f)
#define ZNEAR 0.1f
#define ZFAR 10000.0f



class RAxis{
 public:
  RAxis(glm::mat4 *MVP, glm::vec3 *campos, glm::vec3 origin, RConfig cfg);
  void draw();


 private:
  RTextRenderer Xtext, Ytext, Ztext;
  RShaderProgram pr;
  glm::mat4 *MVP;
  float axislength;
  glm::vec3 *campos;
  glm::vec3 origin;
  VAO dummy_vao;
};



class RRenderer{
protected:
  RConfig cfg;
  
  RAxis axis;
  RBox box;
  glm::mat4 MVP, model, view, proj;
  RTextRenderer textRenderer;
  float gscale;
  ParticleData particles; //Current particle data in CPU
  
public:
  RRenderer(RConfig cfg, float gscale);

  virtual void handle_event(SDL_Event &e);
  
  virtual void update();
  virtual void draw() = 0;
  void display();
  virtual void drawText(const char* text, int x, int y);
  virtual void handle_resize();
  virtual void rotate_model(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
  
  virtual bool upload_instances(ParticleData pdata) = 0;
  
  virtual int pick(int x, int y, int pickindex);

  virtual Uint8 *getPixels();
  virtual glm::int2 getSize();

  FreeCamera cam;
  int picked[2];  

};





#endif
