#ifndef RGLHANDLE_H
#define RGLHANDLE_H

#include "header.h"
#include "RGL.h"
#include "Camera.h"
#include "RFile.h"

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
  bool init(int maxN, RConfig cfg);
  bool init_buffers();
  bool init_sphere();
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

  int pick(int x, int y);
  Uint8 *getPixels();
  glm::int2 getSize(){return fbo.getSize();}

  FreeCamera cam;
 private:
  
  FBO fbo;

  VBO sphere_vbos[2]; //Vertex, index
  VBO instances_vbos[3]; //pos, color, radius
  int maxN, currentN;
  VAO spheres_vao;
  std::map<string, uint> attribs;
  RShaderProgram pr;
  glm::mat4 MVP, model, view, proj;
  GLuint uniMVP, unimodel;
  int picked;
  RConfig cfg;  
};

void fill_sphere_vbos(VBO &posVBO, VBO &indicesVBO);

#endif
