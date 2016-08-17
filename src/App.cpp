#include "App.h"
#include"RPNG.h"
uint palette_id=923302100; //1 is also cool 



App::App(int argc, char *argv[]):
  w(nullptr),
  gl(nullptr)
{
  file.name = argv[1];
  cfg.set_default();
  cfg.parse_args(argc, argv);
  init();
  run();
}
App::~App(){
  delete w;
  delete gl;
}

bool App::init(){
  read_input();
  initSDL();
  initOpenGL();
  visible = true;
  return true;
}


bool App::read_input(){
  if(!file.get_config()){printf("INVALID FILE!\n"); return false;}
  file.read_frames();
  return true;
}

bool App::initSDL(){
  initWindow();
  return true;
}
bool App::initWindow(){
  printf("Starting graphic context...      \n");
  string title = "Superpunto v2.0 WIP! -- "+to_string(file.Nframes)+" frames loaded -- ";  
  w = new RWindow(title, FWIDTH, FHEIGHT);
  printf("DONE!\n");
  return true;
}
bool App::initOpenGL(){
  gl = new RGLHandle(file.maxN, 1.0f/file.maxScale, cfg);
  gl->cam.warp(glm::vec3(0, (file.max_dist[0].y+1.0f)*6.0f/file.maxScale, 0));
  upload_frame(0);
  return true;
}

void App::upload_frame(int frame){
  float *ps, *cs, *ss; //poss, colors, scales
  int N;
  if(!file.get_frame(ps, cs, ss, N, frame)) return;
  gl->upload_instances(ps, cs, ss, N);
}
void App::upload_next_frame(){
  float *ps, *cs, *ss; //poss, colors, scales
  int N;
  if(!file.get_next_frame(ps, cs, ss, N)) return;
  gl->upload_instances(ps, cs, ss, N);
}
void App::upload_previous_frame(){
  float *ps, *cs, *ss; //poss, colors, scales
  int N;
  if(!file.get_previous_frame(ps, cs, ss, N)) return;
  gl->upload_instances(ps, cs, ss, N);
}


void App::run(){
  SDL_Delay(100);
  //This **appears** to fix the damn glitching screen bug, nvm is OS's fail
  w->display();
  w->display();

  while(w->isOpen()){
    handle_events();
    if(w->ready_to_draw()){
      update();
      draw();
      if(cfg.record_movie) screenshot();
    }
  }
}

void App::handle_events(){
  SDL_Event e;
  while(SDL_PollEvent(&e) !=0){
    if(e.type == SDL_QUIT) w->close();
    if(e.type == SDL_KEYDOWN){
      IF_KEY(ESCAPE, w->close();)
      IF_KEY(LALT, gl->cam.set_origin();)
      IF_KEY(SPACE, upload_next_frame();)
      IF_KEY(r, upload_previous_frame();)
      IF_KEY(b, upload_frame(0);)
      IF_KEY(t, upload_frame(file.Nframes-1);)
      IF_KEY(m, cfg.play = !cfg.play;)
      IF_KEY(h, cfg.print_help();)
      IF_KEY(c, this->screenshot();)

      IF_KEY(4, gl->rotate_model(0.1f,1,0,0);)
      IF_KEY(5, gl->rotate_model(0.1f,0,1,0);)
      IF_KEY(6, gl->rotate_model(0.1f,0,0,1);)
      IF_KEY(1, gl->rotate_model(-0.1f,1,0,0);)
      IF_KEY(2, gl->rotate_model(-0.1f,0,1,0);)
      IF_KEY(3, gl->rotate_model(-0.1f,0,0,1);)

	IF_KEY(8, gl->SSAOrad(+0.01);)
	IF_KEY(9, gl->SSAOrad(-0.01);)

    }
    if(e.type == SDL_WINDOWEVENT){
      switch(e.window.event){
      case SDL_WINDOWEVENT_FOCUS_GAINED: visible = true; break;
      case SDL_WINDOWEVENT_FOCUS_LOST: visible = false; break;
      case SDL_WINDOWEVENT_RESIZED:
	FWIDTH = e.window.data1;
	FHEIGHT = e.window.data2;
	gl->handle_resize(); 
	break;	
      }
    }
    if(e.type == SDL_MOUSEBUTTONDOWN){
      int id = gl->pick(e.button.x, e.button.y);
      if(id>=0 && id<file.maxN){
	selected_particle = id;
	cerr<<"Selected Superpunto: "<<selected_particle<<endl;
      }
      
    }
  }
}

void App::screenshot(){
  static int counter = 0;
  static bool init = false;
  if(!init){
    int sysret = system("mkdir -p screenshots");
    init = true;
  }
  std::string fileName = string("screenshots/shot_")+to_string(counter)+string(".png");
  Uint8* pixels = gl->getPixels();
  glm::int2 s = gl->getSize();
  savePNG(fileName.c_str(), pixels, s.x, s.y);
  counter++;
}

void App::update(){
  if(visible) gl->update();
  if(cfg.play) upload_next_frame();
}
void App::draw(){
    w->update_fps();
    gl->draw();
    w->display();
}


