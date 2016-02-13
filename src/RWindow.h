#ifndef RWINDOW_H
#define RWINDOW_H
#include "header.h"
#include "RGL.h"

#include<string>

class RWindow{
public:
  RWindow(std::string title, uint fw, uint fh);
  ~RWindow();
  void display();
  bool ready_to_draw();
  void update_fps();
  bool isOpen();
  void close();
private:
  SDL_Window* w;
  SDL_Surface* scsurf;
  bool open;
  RGLContext glcontext;

  float FPS;
  std::string title;
};

#endif
