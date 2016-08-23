#ifndef APP_H
#define APP_H

#include"header.h"
#include"RWindow.h"
#include"RGLHandle.h"
#include"RFile.h"
#include<vector>
#include<string>
#include<iostream>

using namespace std;


class App{
public:
  App(int argc, char *argv[]);
  ~App();
  bool init();
  void run();
  
private:
  bool read_input();
  bool initSDL();
  bool initWindow();
  bool initOpenGL();
  
  void upload_frame(int frame);
  void upload_next_frame();
  void upload_previous_frame();

  void update();
  void handle_events();

  void draw();

  void screenshot();
  void movieAddFrame();

  RGLHandle *gl;
  RWindow *w;
  RConfig cfg;
  RFile file;

  
  bool visible;

};

#endif
