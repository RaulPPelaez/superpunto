#include"RWindow.h"


uint FWIDTH = 800;
uint FHEIGHT = 800;

RWindow::RWindow(string title, uint fw, uint fh){
  if(SDL_WasInit(SDL_INIT_VIDEO) == 0) SDL_Init(SDL_INIT_VIDEO);  
  this->title = title;
  w = SDL_CreateWindow(title.c_str(), 0, 0, fw, fh, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  glcontext.init(w);
  open = true;
}
RWindow::~RWindow(){
  SDL_DestroyWindow(w);
  SDL_Quit();
}
void RWindow::display(){ 
SDL_GL_SwapWindow(w);
}
bool RWindow::isOpen(){ return this->open;}
void RWindow::close(){ open=false;}

void RWindow::update_fps(){
  static int frames_this_second = 0;
  static int counted_seconds = SDL_GetTicks()/1000;
  frames_this_second++;

  int total_seconds = SDL_GetTicks()/1000; 

  if(total_seconds>counted_seconds){
    FPS = frames_this_second/(total_seconds-counted_seconds);
    counted_seconds = total_seconds;
    SDL_SetWindowTitle(w, (title+" FPS = "+to_string((int)(FPS+0.5))).c_str());
    frames_this_second = 0;
  }

}

bool RWindow::ready_to_draw(){
  return true;
  /*
    static int last_time = 0;
    int current_time = SDL_GetTicks();
    if(current_time>(100.0f/3.0f)+last_time){
      last_time = current_time;
      return true;
    }
    return false;
  */
}
