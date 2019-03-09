#ifndef RTEXTRENDERER_H
#define RTEXTRENDERER_H
#include"defines.h"
#include"RGL.h"
#include<SDL2/SDL_ttf.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "RWindow.h"
namespace superpunto{

  class RTextRenderer{    
  public:
    RTextRenderer(std::shared_ptr<System> sys, std::shared_ptr<RWindow> w);
    ~RTextRenderer();
    bool setFont(const char * fontName, int size);
    bool setText(const char* text, int x, int y, float sizefactor = 1.0f);
    void handle_resize(uint fw, uint fh);
    bool move(int x, int y);
    void draw();
  
  private:
    std::shared_ptr<System> sys;
    static std::shared_ptr<RShaderProgram> pr;
    static std::shared_ptr<VAO> dummy_vao;
    glm::vec2 size, pos;
    float size_factor;
    RTex tex;
    TTF_Font *font;
    static int instance_counter;
    std::shared_ptr<RWindow> w;
  };


}
#endif
