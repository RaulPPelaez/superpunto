#ifndef RGLHANDLE_H
#define RGLHANDLE_H

#include "header.h"
#include "RGL.h"
#include "Camera.h"
#include "RFile.h"
#include "RTextRenderer.h"

#include<string>
#include<map>
#include<iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
class RGLHandle{
 public:
  RGLHandle(int maxN, float gscale, RConfig cfg);
  ~RGLHandle();
  bool init_buffers();
  bool init_sphere();
  bool init_instance_vbos();
  bool init_vao();

  bool init_shaders();

  bool init_math();
  bool init_uniforms();
  
  bool upload_instances(ParticleData pdata);

  void update();
  void geometry_pass();
  void light_pass();
  void SSAO_pass();
  void SSAOrad(float inc);

  void render_picked();
  void draw();
  void drawText(const char* text, int x, int y);
  
  void handle_resize();
  void rotate_model(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);

  int pick(int x, int y, int pickindex);
  Uint8 *getPixels();
  glm::int2 getSize(){return gBuffer.getSize();}

  FreeCamera cam;
  int picked[2];
 private:

  void render_picked();
  void render_box();
  
  FBO fbo, ssaofbo;
  GBuffer gBuffer;

  VBO sphere_vbos[2]; //Vertex, index
  VBO instances_vbos[3]; //pos, color, radius

  ParticleData particles; //Current particle data in CPU
  VBO lines_vbo; //lines start/end
  VBO box_vbo;
  int maxN;
  float gscale;
  glm::vec3 boxSize;
  VAO spheres_vao, dummy_vao, line_vao, box_vao;
  std::map<string, uint> attribs;
  RShaderProgram pr, lightpr, ssaopr, linepr, boxpr;
  glm::mat4 MVP, model, view, proj;
  GLuint uniMVP, unimodel;
  bool picking;

  RConfig cfg;

  RTextRenderer textRenderer;
};

void fill_sphere_vbos(VBO &posVBO, VBO &indicesVBO);
void fill_box_vbo(VBO &boxVBO);
#endif
