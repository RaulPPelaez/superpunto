#ifndef RGL_H
#define RGL_H


#include <defines.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <SFML/Graphics.hpp>
using namespace sf;

#include <stdlib.h>


#include<glib.h>
#include<RModelHandler.h>
#include<RPostProcessor.h>
#include<RShadowMapper.h>
#include<Camera.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class RGL{
 public:
 RGL():cam(){}
  ~RGL(){
    glDeleteBuffers(3,instancing_vbos);
  }
  void initialize();
  void draw();

  void update();

  void handle_event(sf::Event event);

  void rotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
  void scale(GLfloat x, GLfloat y, GLfloat z);
  void translate(GLfloat x, GLfloat y, GLfloat z);

  void lookAt(const glm::vec3 &pos, const glm::vec3 &view, const glm::vec3 &up);

  void loadIdentity();
  void upload_MVP();

  void setUniform(const char* name, float val);
  
  void draw_to_fb();
  void post_process();
 private:

 protected:
  virtual void initBuffers()=0;
  virtual void upload_step()=0;
  glm::mat4 proj, model, view, MVP;
  FreeCamera cam;

  glm::vec3 lightpos;
  float frames;
  RModelHandler drawables;
  GLuint instancing_vbos[3];
  
  RPostProcessor post_processor;
  RShadowMapper shadow_mapper;

};


#endif
