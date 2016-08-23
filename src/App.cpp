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
  if(!file.set_frame(frame)) return;
  gl->upload_instances(file.current);
  gl->drawText(
	       (file.msgs[file.current_frame]+
		string(" -- ")+to_string(file.current_frame)).c_str(),
	       0, 0);
}
void App::upload_next_frame(){
  if(!file.set_next_frame()) return;
  gl->upload_instances(file.current);
  gl->drawText(
	       (file.msgs[file.current_frame]+
		string(" -- ")+to_string(file.current_frame)).c_str(),
	       0, 0);
    
}
void App::upload_previous_frame(){
  if(!file.set_previous_frame()) return;
  gl->upload_instances(file.current);
  gl->drawText(
	       (file.msgs[file.current_frame]+
		string(" -- ")+to_string(file.current_frame)).c_str(),
	       0, 0);

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
      IF_KEY(l, cfg.record_movie = !cfg.record_movie; cfg.play = !cfg.play;)	

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
      int button = e.button.button == SDL_BUTTON_LEFT ? 0:1;
      int id = gl->pick(e.button.x, e.button.y, button);
      gl->picked[button] = id;
      if(id>=0 && id<file.maxN)
	cerr<<"Selected Superpunto: "<<id<<endl;
      if(gl->picked[0]>=0 && gl->picked[1] >=0){
	cerr<<"\tDistance between "<<gl->picked[0]<<" and "<<gl->picked[1]<<": ";
	float *ps = file.current.pos;
	cerr<<"( ";
	fori(0,3)
	  cerr<<ps[3*gl->picked[1]+i]-ps[3*gl->picked[0]+i]<<" ";
	cerr<<")"<<endl;
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
  std::string fileName = string("screenshots/shot_")+
    to_string(counter)+string(".png");
  Uint8* pixels = gl->getPixels();
  glm::int2 s = gl->getSize();
  savePNG(fileName.c_str(), pixels, s.x, s.y);
  counter++;
}

void App::movieAddFrame(){
  static FILE* fp = nullptr;
  static bool init = false;
  if(!init){
    int rate = 16;
    int bitrate = 24000;
    string movie_fileName("movie.mp4");
    string cmd = string("avconv -loglevel panic -y -f rawvideo -s ");
    cmd += to_string(FWIDTH) + string("x") + to_string(FHEIGHT);
    cmd += string(" -pix_fmt rgba -r ") + to_string(rate);
    cmd += string(" -i - -vf vflip -an -b:v ") + to_string(bitrate) +
      string("k ");
    cmd += movie_fileName;
    cerr<<"Encoding video with command:"<<endl;
    cerr<<cmd<<endl;
    fp = popen(cmd.c_str(), "w");
    init = true;
  }
  Uint8* pixels = gl->getPixels();
  glm::int2 s = gl->getSize();
  fwrite(pixels, s.x*s.y*4, 1, fp);
}
void App::update(){
  if(visible) gl->update();
  if(cfg.record_movie) movieAddFrame();
  if(cfg.play) upload_next_frame();
}
void App::draw(){
    w->update_fps();
    gl->draw();
    w->display();
}


