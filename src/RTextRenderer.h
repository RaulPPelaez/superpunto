#ifndef RTEXTRENDERER_H
#define RTEXTRENDERER_H
#include"header.h"
#include"RGL.h"
#include<SDL2/SDL_ttf.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class RTextRenderer{
public:
  RTextRenderer();
  bool setFont(const char * fontName, int size);
  bool setText(const char* text, int x, int y);
  void draw();
  
private:
  RShaderProgram pr;
  VAO dummy_vao;
  glm::vec2 size, pos;
  RTex tex;
  TTF_Font *font;
  static bool SDL_ttf_init;
};



#endif
