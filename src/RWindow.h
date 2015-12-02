#ifndef RWINDOW_H
#define RWINDOW_H
#include<SFML/Graphics.hpp>
#include<sstream>
#include"defines.h"

using namespace sf;

class RWindow: public sf::RenderWindow{
 public:
  RWindow(){
    frames=0;
    FPS=TARGET_FPS;
    forced_frame=false;
  }
  void update_fps(){
    frames++;
    float time = fps_clock.getElapsedTime().asMicroseconds();
    if(time>=1e6){
      FPS = (float)frames;
      frames = 0;
      fps_clock.restart();
      std::stringstream sstr;
      sstr<<" FPS = "<<(int)(FPS+0.5);
      this->setTitle(sstr.str());
    }
  }
  void update_fps(const std::string &additional){
    frames++;
    float time = fps_clock.getElapsedTime().asMicroseconds();
    if(time>=1e6){
      FPS = (float)frames;
      frames = 0;
      fps_clock.restart();
      std::stringstream sstr;
      sstr<<" FPS = "<<(int)(FPS+0.5)<<" "<<additional;
      this->setTitle(sstr.str());
    }
  }
  bool ready_to_draw(){
    if(forced_frame){forced_frame=false; return true;}
    float time = draw_clock.getElapsedTime().asMicroseconds();
    if(time>=(1e6/TARGET_FPS)){
      draw_clock.restart();
      return true;
    }
    return false;
  }
  unsigned int *updates_per_frame;
  void force_draw(){
    forced_frame=true;
  }
 private:
  int frames;
  float FPS;
  Clock fps_clock;
  Clock draw_clock;
  bool forced_frame;
};
#endif
