#ifndef RBOX_H
#define RBOX_H
#include "RGL.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class RBox{
public:
  RBox(float gscale, glm::mat4 *MVP, glm::vec3 size=glm::vec3(0,0,0));
  void setSize(glm::vec3 L);
  void draw();

private:
  RShaderProgram pr;
  VAO vao;
  VBO vbo;
  float gscale;
  glm::vec3 size;
  glm::mat4 *MVP;
};
void fill_box_vbo(VBO &boxVBO);

#endif
