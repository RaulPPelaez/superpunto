#ifndef APP_H
#define APP_H

#include "Camera.h"
#include "RFile.h"
#include "RRenderer.h"
#include "RWindow.h"
#include "System.h"
#include "defines.h"
#include <memory>
#include "MovieRecorder.h"
#include <unordered_map>
namespace superpunto {
class App {
public:
  App(std::shared_ptr<System>, int argc, char *argv[]);
  ~App();
  void run();

private:
  void read_input();
  void initWindow();
  void initOpenGL();

  void setFrame(int frame);
  void setNextFrame();
  void setPreviousFrame();

  void update();
  void handle_events();

  void draw();

  void screenshot();
  void movieAddFrame();
  void movieStop();

  std::shared_ptr<RRenderer> gl;
  using Camera = FreeCamera;
  std::shared_ptr<FreeCamera> cam;
  std::shared_ptr<RWindow> w;
  std::shared_ptr<RFile> file;
  std::shared_ptr<System> sys;
  std::unique_ptr<MovieRecorder> movie;
  std::unordered_map<SDL_Keycode, Uint32> last_key_time;
  Uint32 repeatDelay = 50;    // milliseconds
  Uint32 repeatInterval = 50;
  int current_frame = -1;
  bool visible;
  bool play = false;
  bool record_movie = false;
  glm::vec3 initial_camera_position, initial_camera_center;
};
} // namespace superpunto
#endif
