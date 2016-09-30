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
  bool setText(const char* text, int x, int y, float sizefactor = 1.0f);
  bool move(int x, int y);
  void draw();
  
private:
  static RShaderProgram* pr;
  static VAO* dummy_vao;
  glm::vec2 size, pos;
  float size_factor;
  RTex tex;
  TTF_Font *font;
  static bool SDL_ttf_init;
};



#endif
