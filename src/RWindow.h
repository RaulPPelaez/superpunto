#ifndef RWINDOW_H
#define RWINDOW_H
#include "defines.h"
#include "RGL.h"
#include"System.h"
#include<string>

namespace superpunto{
  class RWindow{
  public:
    RWindow(std::shared_ptr<System> sys, std::string title, uint fw, uint fh);
    //RWindow(std::string title, uint fw, uint fh);
    ~RWindow();
    void display();
    bool ready_to_draw();
    void update_fps();
    bool isOpen();
    void close();
    glm::int2 getResolution(){ return resolution;}
    void handle_resize(uint new_fw, uint new_fh){
      resolution = {new_fw, new_fh};
    }
  private:
    SDL_Window* w;
    SDL_Surface* scsurf;
    bool open;
    std::shared_ptr<RGLContext> glcontext;

    glm::int2 resolution;
    float FPS;
    std::shared_ptr<System> sys;
    std::string title;

  };
}
#endif
