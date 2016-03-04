#ifndef RGLHANDLE_H
#define RGLHANDLE_H

#include "header.h"
#include "RGL.h"
#include "Camera.h"
#include<string>
#include<map>
#include<iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
class RGLHandle{
 public:
  RGLHandle();
  ~RGLHandle();
  bool init(int maxN);
  bool init_buffers();
  bool init_instance_vbos();
  bool init_vao();

  bool init_shaders();

  bool init_math();
  bool init_uniforms();
  
  bool upload_instances(const float *pos, const float *colors, const float *scales, int N);

  void update();
  void draw();
  
  void handle_resize();
  void rotate_model(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);


  FreeCamera cam;
 private:

  VBO instances_vbos[3]; //pos, color, radius
  int maxN, currentN;
  VAO spheres_vao;
  std::map<string, int> attribs, uniforms;
  RShaderProgram pr;

  glm::mat4 MVP, model, view, proj;


};


#endif
