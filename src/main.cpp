#include<string>
#include<vector>
#include <defines.h>
#include<glib.h>
#include <GL/glext.h>
#include<RModelHandler.h>
#include<Camera.h>
#include<RWindow.h>
#include<RGL.h>
#include"utils.h"
#include "gif.h"
#include"helper.h"

using namespace std;
using namespace sf;

std::string fileName;
 
class RGLContext: public RGL{
public:
  ~RGLContext(){    }
  
  void update();
  void draw();
  void handle_event(sf::Event event);
  void set_box(float bsize);  
  std::string current_msg();

  bool play_movie;  
  std::vector<vector<float>> positions, colors,scales;
  std::vector<float> max_dist;
  std::vector<std::string> msgs; //Mesages in the comments #L=x bolablabla
  std::vector<float> Lbox; //Custom box size in the comments  
  int Nframes;
  int current_step;
private:	
  void initBuffers();
  void upload_step();
  
  float parse_comment(std::string line, std::string &msg);
};

void RGLContext::draw(){
  RGL::draw();    
  drawables.draw_lines();
  post_process();
}

void RGLContext::update(){
  RGL::update(); 
  if(play_movie)upload_step();
}

void RGLContext::handle_event(sf::Event event){
  RGL::handle_event(event);
 if (event.type == Event::KeyPressed){
   IF_KEY(R, current_step= Rmax(current_step-2,0);upload_step();)
   IF_KEY(M, play_movie = !play_movie;)
     IF_KEY(T, current_step = Nframes-1; upload_step();)
 }
}

float RGLContext::parse_comment(std::string line, std::string &msg){

  std::string delim = "L=";
  std::string delim2 = ";";
  float L = 0.0;
  size_t pos = 0;
  std::string token;
  pos = line.find(delim);
  if(pos!=std::string::npos){
    line.erase(0, pos + delim.length());
    pos = line.find(delim2);
    token = line.substr(0,pos);
    L = std::stof(token, NULL);
    line.erase(0, pos+1);

  }
  msg = line;
  return L;
}
std::string RGLContext::current_msg(){
  return msgs[current_step-1];
}
void RGLContext::initBuffers(){
play_movie = false;
 
  ifstream in(fileName.c_str());
  FileConfig fc = get_config(fileName.c_str());
    
  std::string line;  
  
  int c;
  int Nmax=fc.maxN;
  Nframes = fc.nframes;
  positions.resize(fc.nframes);
  scales.resize(fc.nframes);
  colors.resize(fc.nframes);
  max_dist.resize(fc.nframes);
  msgs.resize(fc.nframes);
  Lbox.resize(fc.nframes);
  
  

  positions[0].resize(3*fc.N[0],0);
  scales[0].resize(fc.N[0],1);
  colors[0].resize(3*fc.N[0],1);    
  max_dist[0] = 0.0;

  getline(in,line);
  if(iscomment(line)){ Lbox[0] = parse_comment(line, msgs[0]); getline(in,line);}
  else{Lbox[0] = 0.0; msgs[0] = " ";}
  std::stringstream is(line);
  
  double temp[fc.nrows];
  vector<int> ctemp(fc.N[0],0);
  int frame = 0;
  int N = -1;
  printf("\rLoading data... %d%%   ", 0);
  fflush(stdout);
  while(!in.eof()){
    if(!iscomment(line)) N++;
   else{
     colors[frame] = parse_colors(ctemp);
     N=-1; frame++; 
     Lbox[frame] = parse_comment(line, msgs[frame]);
     max_dist[frame] = 0.0;
     positions[frame].resize(3*fc.N[frame],0);
     scales[frame].resize(fc.N[frame],1);
     //colors[frame].resize(3*fc.N[frame],1);    
     ctemp.resize(fc.N[frame],1);
     getline(in,line);
     printf("\rLoading data... %d%%   ", (int)(100.0f*(float)frame/(float)fc.nframes +0.5f));
     fflush(stdout);
     continue;
   }
   is.clear();
   is.str(line);
   for(int i=0; i<fc.nrows;i++) is>>temp[i];
   if(fc.nrows==3){temp[3] = 1; temp[4] = 1;}
   if(fc.nrows==4){temp[4] = temp[3]; temp[3]=1;}
   fori(0,3){
     positions[frame][3*N+i] = temp[i];
     max_dist[frame] = Rmax(max_dist[frame], abs(temp[i]));
   }
   scales[frame][N] = temp[3];
   ctemp[N] = (temp[4]+1)*39275;
   
   getline(in,line);
  }
  printf("\rLoading data... 100 %%   \nDONE!\n\nSetting up graphic enviroment...");
  colors[frame] = parse_colors(ctemp);

  
  glGenBuffers(3, instancing_vbos);
  
  drawables.set_instancing("color", instancing_vbos[1], 0, 3, 3*sizeof(float));
  drawables.set_instancing("scale", instancing_vbos[2], 0, 1, sizeof(float));
  drawables.set_instancing("pos", instancing_vbos[0], positions[0].size()/3, 3, 3*sizeof(float));
  
  float max = Rmax(max_dist[0], Lbox[0]);
  
  cam.pos.x =0;
  cam.pos.y +=2*( max+10);
  cam.pos.z -=2*( max+10);
  
  current_step = 0;
  upload_step();
}

void RGLContext::set_box(float bsize){
  GLfloat *temp = get_wired_cube();
  GLfloat box_data[72];
  fori(0,72) box_data[i] = temp[i];
  drawables.Nlines = 12; //2 points/line, 3 coords/point                                              
  fori(0,72) box_data[i] -= 0.5;
  fori(0,72) box_data[i] *= bsize;

  glBindBuffer(GL_ARRAY_BUFFER, drawables.line_vbo);
  glBufferData(GL_ARRAY_BUFFER, 12*2*3*sizeof(float), box_data, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void RGLContext::upload_step(){

 int frame = current_step;
  if(current_step<Nframes)current_step++;

  glBindBuffer(GL_ARRAY_BUFFER, instancing_vbos[2]);
  glBufferData(GL_ARRAY_BUFFER, scales[frame].size()*sizeof(float), scales[frame].data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, instancing_vbos[1]);
  glBufferData(GL_ARRAY_BUFFER, colors[frame].size()*sizeof(float), colors[frame].data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, instancing_vbos[0]);
  glBufferData(GL_ARRAY_BUFFER, positions[frame].size()*sizeof(float), positions[frame].data(), GL_DYNAMIC_DRAW);
  
  glBindBuffer(GL_ARRAY_BUFFER,0);
  
  if(Lbox[frame] == 0.0)set_box(2*max_dist[frame]);
  else{ set_box(2*Lbox[frame]); }
  
}

class App{
  public:
    App(int argc, char** argv);
    void Run();
    void Exit();
    void screenshot();
    void record_frame();
  private:
    void draw();
    void handle_events();
    RWindow window;	
    RGLContext glcontext;
    unsigned int updates_per_frame;
    bool dostep;
    bool pause;
  unsigned int shot_counter, frame_shot_counter, frames_between_screenshots;
  bool record, record_movie;
    unsigned int frame_counter;
    std::vector<sf::Image> GIF;
    GifWriter GIF2;
    sf::Font font;
    sf::Text text;

};

App::App(int argc, char** argv){
  cout<<"INIT...";
  glewExperimental = GL_TRUE;
  record_movie = false;
  float R,G,B;
  R = G = B = 0.5;
  fori(0,argc){
    if(strcmp(argv[i],"--record")==0) record_movie = true;
    if(strcmp(argv[i],"--frames-between-screenshots")==0)frames_between_screenshots = atoi(argv[i+1]);
    if(strcmp(argv[i],"--background")==0){
      R = stod( argv[i+1]);
      G = stod( argv[i+2]);
      B = stod( argv[i+3]);
    }
    if(strcmp(argv[i],"-h")==0){
      printf("Usage:\n spunto file [opts]\n");
      printf("Options:\n");
      printf("--record :  Makes a movie of all the frames in file and generates a .gif\n");
      printf("--frames-between-screenshots X : Number of frames skipped between screenshots when recording (default = 2)\n");
      printf("--background R G B : Background color in RGB, default R=G=B=0.5\n");

    }
  }

  glewInit();

  ContextSettings context(24, 8, 2, 0, 3);
  window.create(VideoMode(FWIDTH,FHEIGHT), "Superpunto",Style::Default, context);
  cout<<"OpenGL Version used "<<window.getSettings().majorVersion<<endl;
  //window.setPosition(Vector2i(0,0));
  updates_per_frame = 0;
  
  window.setActive(true);
  
  if(TARGET_FPS==60) window.setVerticalSyncEnabled(true);

  glClearColor(R, G, B, 1.0f);   
  
  sf::Mouse::setPosition(sf::Vector2i(FWIDTH/2, FHEIGHT/2));
  shot_counter = frame_shot_counter = 0;
  record = false;
  frame_counter = 0;
  frames_between_screenshots = 1;

  glcontext.initialize(); 
  pause = true;
  dostep = false;

  font.loadFromFile("/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf");
  text.setFont(font); 
  cout<<"DONE!"<<endl;
}
void App::handle_events(){
  Event event;
  while (window.pollEvent(event)){
    glcontext.handle_event(event);
    if (event.type == Event::Closed) window.close();
    if (event.type == Event::Resized){
      int w = Rmin(event.size.width, event.size.height);
      glViewport(0.0, 0.0, w, w);
    }
    if (event.type == Event::KeyPressed){
      IF_KEY(Escape, window.close();)
      IF_KEY(N, dostep = true; pause = !pause;)
      IF_KEY(L, record = !record; glcontext.play_movie = true;)
      IF_KEY(C, screenshot();)
      IF_KEY(LAlt, sf::Mouse::setPosition(sf::Vector2i(FWIDTH/2, FHEIGHT/2));)
    }
  }
  
}
void App::Run(){
  if(record_movie){
    printf("Recording...\n");
    record = true;
    glcontext.play_movie = true;
    while(glcontext.current_step<glcontext.Nframes){
      draw();
    }
    window.close();
  }
 
  while(window.isOpen()){ 
    draw();
  }
}
void App::draw(){
  if(window.ready_to_draw()){
    handle_events();
    window.update_fps();
    glcontext.update();
    glcontext.draw();
    frame_counter++;
    if(record)if(frame_counter%(frames_between_screenshots+1)==0) this->record_frame();
    
    text.setString(glcontext.current_msg());
    window.draw(text);
    window.display();
    
  }
}


void App::record_frame(){
  sf::Image s = window.capture();
  if(frame_shot_counter==0)  GifBegin(&GIF2, "movie.gif",s.getSize().x, s.getSize().y, 1);
  GifWriteFrame(&GIF2, s.getPixelsPtr(), s.getSize().x, s.getSize().y, 1);
  frame_shot_counter++;
  cout<<"Frame "<<frame_shot_counter-1<<" added to the gif!"<<endl;
}


void App::screenshot(){
  sf::Image s = window.capture();
  std::stringstream is;
  is<<"shot_"<<shot_counter<<".png";
  s.saveToFile(is.str().c_str());
  shot_counter++;
  cout<<"Screenshot "<<shot_counter-1<<" saved!"<<endl;
}

void App::Exit(){
  
}
int main(int argc, char** argv){
  fileName = std::string(argv[1]);
  App app(argc,argv);
  app.Run();
  app.Exit();
  printf("Properly exiting...DONE!\nHasta la vista!\n");
 return 0;
}
