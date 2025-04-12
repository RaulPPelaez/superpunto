#include "App.h"
#include "RPNG.h"
#include "RParticleRenderer.h"
// #include"RArrowRenderer.h"

namespace superpunto {

App::App(std::shared_ptr<System> sys, int argc, char *argv[])
    : visible(true), sys(sys) {

  auto op = sys->getInputOptions();
  file = std::make_shared<RFile>(sys);
  initWindow();
  initOpenGL();
}
App::~App() {}

void App::initWindow() {
  sys->log<System::DEBUG>("Starting graphic context...");
  std::string title = "Superpunto v" + std::to_string(SUPERPUNTO_MAJOR) + "." +
                      std::to_string(SUPERPUNTO_MINOR) + "!";
  auto op = sys->getInputOptions();
  w = std::make_shared<RWindow>(sys, title, op.target_FW, op.target_FH);
}
void App::initOpenGL() {
  auto op = sys->getInputOptions();
  cam = std::make_shared<Camera>();
  auto data = file->getCurrentFrameData();

  float3 maxDist = file->getMaxDist();
  float maxScale = file->getMaxScale();
  sys->log<System::DEBUG>("[App] Max radius in frame 0: %g", maxScale);
  this->initial_camera_position =
      glm::vec3(0, (maxDist.y + 1.0f) * 6.0f / maxScale, 0);
  cam->warp(initial_camera_position);
  switch (op.render_type) {
  case RenderType::PARTICLES:
    gl = std::make_shared<RParticleRenderer>(sys, w, cam, 1.0f / maxScale);
    break;
  default:
    sys->log<System::CRITICAL>("Invalid Rederer selected");
  }
  setFrame(0);
}

void App::setFrame(int frame) {
  file->setFrame(frame);
  auto current = file->getCurrentFrameData();
  current_frame = current.frame;
  gl->upload_instances(current);
  gl->drawText(
      (current.msg + std::string(" -- ") + std::to_string(current_frame))
          .c_str(),
      0, 0);
}
void App::setNextFrame() { setFrame(current_frame + 1); }

void App::setPreviousFrame() {
  if (current_frame > 0)
    setFrame(current_frame - 1);
}

void App::run() {
  SDL_Delay(100);
  w->display();
  w->display();
  while (w->isOpen()) {
    handle_events();
    if (w->ready_to_draw()) {
      update();
      draw();
    }
  }
}

void App::handle_events() {
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    gl->handle_event(e);
    if (e.type == SDL_QUIT)
      w->close();
    if (e.type == SDL_KEYDOWN) {
      IF_KEY(ESCAPE, w->close();)
      IF_KEY(LALT, cam->set_origin();)
      IF_KEY(SPACE, setNextFrame();)
      IF_KEY(r, setPreviousFrame();)
      IF_KEY(b, setFrame(0);)
      IF_KEY(t, setFrame(-1);)
      IF_KEY(m, play = !play;)
      IF_KEY(h, sys->printHelp();)
      IF_KEY(c, this->screenshot();)
      IF_KEY(l, record_movie = !record_movie; play = !play;)

      IF_KEY(4, gl->rotate_model(0.1f, 1, 0, 0);)
      IF_KEY(5, gl->rotate_model(0.1f, 0, 1, 0);)
      IF_KEY(6, gl->rotate_model(0.1f, 0, 0, 1);)
      IF_KEY(1, gl->rotate_model(-0.1f, 1, 0, 0);)
      IF_KEY(2, gl->rotate_model(-0.1f, 0, 1, 0);)
      IF_KEY(3, gl->rotate_model(-0.1f, 0, 0, 1);)
      IF_KEY(0, cam->reset_camera_view(); gl->reset_model();
             cam->warp(initial_camera_position);)
    }
    if (e.type == SDL_WINDOWEVENT) {
      switch (e.window.event) {
      case SDL_WINDOWEVENT_FOCUS_GAINED:
        visible = true;
        break;
      case SDL_WINDOWEVENT_FOCUS_LOST:
        visible = false;
        break;
      case SDL_WINDOWEVENT_RESIZED:
        int fw = e.window.data1;
        int fh = e.window.data2;
        gl->handle_resize(fw, fh);
        w->handle_resize(fw, fh);
        break;
      }
    }
    if (e.type == SDL_MOUSEBUTTONDOWN) {
      int button = e.button.button == SDL_BUTTON_LEFT ? 0 : 1;
      int id = gl->pick(e.button.x, e.button.y, button);
      gl->picked[button] = id;
      auto current = file->getCurrentFrameData();
      const float *ps = current.pos;
      if (id >= 0 && id < current.N) {
        float3 pi = {ps[3 * gl->picked[button]], ps[3 * gl->picked[button] + 1],
                     ps[3 * gl->picked[button] + 2]};
        sys->log<System::MESSAGE>("Selected Superpunto: %d , at %g %g %g", id,
                                  pi.x, pi.y, pi.z);
      }
      if (gl->picked[0] >= 0 && gl->picked[1] >= 0) {
        float rij[3];
        fori(0, 3) rij[i] =
            ps[3 * gl->picked[1] + i] - ps[3 * gl->picked[0] + i];
        sys->log<System::MESSAGE>(
            "Distance between %d and %d: %f %f %f ; Modulus: %g", gl->picked[0],
            gl->picked[1], rij[0], rij[1], rij[2],
            sqrt(rij[0] * rij[0] + rij[1] * rij[1] + rij[2] * rij[2]));
      }
    }
  }
}

void App::screenshot() {
  static int counter = 0;
  static bool init = false;

  if (!init) {
    int sysret = system("mkdir -p screenshots");
    init = true;
  }
  std::string fileName = std::string("screenshots/shot_") +
                         std::to_string(counter) + std::string(".png");
  Uint8 *pixels = gl->getPixels();
  glm::int2 s = gl->getSize();
  sys->log<System::MESSAGE>("Saving screenshot... %s", fileName.c_str());
  sys->log<System::MESSAGE>("Size: %dx%d", s.x, s.x);

  fori(0, w->getResolution().x * w->getResolution().y) pixels[4 * i + 3] = 255;
  savePNG(fileName.c_str(), pixels, s.x, s.y);
  counter++;
}

void App::movieAddFrame() {
  static FILE *fp = nullptr;
  static bool init = false;
  if (!init) {
    int rate = 16;
    int bitrate = 24000;
    std::string movie_fileName("movie.mp4");
    std::string cmd = std::string("avconv -loglevel panic -y -f rawvideo -s ");
    cmd += std::to_string(w->getResolution().x) + std::string("x") +
           std::to_string(w->getResolution().y);
    cmd += std::string(" -pix_fmt rgba -r ") + std::to_string(rate);
    cmd += std::string(" -i - -vf vflip -an -b:v ") + std::to_string(bitrate) +
           std::string("k ");
    cmd += movie_fileName;
    sys->log<System::MESSAGE>("Encoding video with command: %s", cmd.c_str());
    fp = popen(cmd.c_str(), "w");
    init = true;
  }
  Uint8 *pixels = gl->getPixels();
  glm::int2 s = gl->getSize();
  fwrite(pixels, s.x * s.y * 4, 1, fp);
}
void App::update() {
  if (visible)
    gl->update();
  if (record_movie)
    movieAddFrame();
  if (play)
    setNextFrame();
}
void App::draw() {
  w->update_fps();
  gl->draw();
  w->display();
}

} // namespace superpunto
