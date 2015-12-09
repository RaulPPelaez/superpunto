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
#include<stdio.h>
using namespace std;
using namespace sf;

std::string fileName;
int palette_id=923302100; //1 is also cool
 
class RGLContext: public RGL{
public:
  ~RGLContext(){    }
  
  void update();
  void draw();
  void handle_event(sf::Event event);
  void set_box(float bsize);  
  std::string current_msg();

  bool play_movie;  
  std::vector<vector<float>> positions, colors, scales;
  std::vector<float> max_dist;
  std::vector<std::string> msgs; //Mesages in the comments #L=x; bolablabla
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
   IF_KEY(B, current_step = 0; upload_step();)
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
  
  FileConfig fc = get_config(fileName.c_str());
  ifstream in(fileName.c_str());   
  
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

  std::string line;  
  getline(in,line);
  if(iscomment(line)){Lbox[0] = parse_comment(line, msgs[0]); getline(in,line);}
  else{Lbox[0] = 0.0; msgs[0] = " ";}
  std::stringstream is(line);
  
  double temp[fc.nrows];
  vector<int> ctemp(fc.N[0],0);
  int frame = 0;
  int N = -1;
  printf("\rLoading data... %d%%   ", 0);
  fflush(stdout);

  srand(palette_id);
  vector<unsigned int> palette(1000,0);
  fori(0,1000) palette[i] = rand()%16581375;
  while(!in.eof()){
    if(!iscomment(line)) N++;
   else{
     colors[frame] = parse_colors(ctemp);
     N=-1; frame++; 
     Lbox[frame] = parse_comment(line, msgs[frame]);
     max_dist[frame] = 0.0;
     positions[frame].resize(3*fc.N[frame],0);
     scales[frame].resize(fc.N[frame],1);
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
   ctemp[N] = palette[((int)temp[4]+1)%1000];
   
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
  static bool first_time = true;
  if(first_time){
    GLfloat *temp = get_wired_cube();
    GLfloat box_data[72];
    fori(0,72) box_data[i] = temp[i];
    drawables.Nlines = 12; //2 points/line, 3 coords/point                           
    drawables.set_line_uniform(bsize);
    glBindBuffer(GL_ARRAY_BUFFER, drawables.line_vbo);
    glBufferData(GL_ARRAY_BUFFER, 12*2*3*sizeof(float), box_data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    first_time = false;
  }
  else{
    drawables.set_line_uniform(bsize);
  }
}


void RGLContext::upload_step(){
 int frame = current_step;
  if(current_step<Nframes)current_step++;
  else{play_movie=false;return;}
  glBindBuffer(GL_ARRAY_BUFFER, instancing_vbos[2]);
  glBufferData(GL_ARRAY_BUFFER, scales[frame].size()*sizeof(float), scales[frame].data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, instancing_vbos[1]);
  glBufferData(GL_ARRAY_BUFFER, colors[frame].size()*sizeof(float), colors[frame].data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, instancing_vbos[0]);
  glBufferData(GL_ARRAY_BUFFER, positions[frame].size()*sizeof(float), positions[frame].data(), GL_DYNAMIC_DRAW);
  
  glBindBuffer(GL_ARRAY_BUFFER,0);
  
  if(Lbox[frame] == 0.0)set_box(2*max_dist[frame]);
  else{ set_box(2*Lbox[frame]); }
  //  CheckGLError();
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
  void init_default_params();
  void parse_input(int argc, char** argv);

  RWindow window;	
  RGLContext glcontext;
  std::vector<sf::Image> GIF;
  GifWriter GIF2;
  sf::Font font;
  sf::Text text;
  //Configuration parameters ---------
  unsigned int updates_per_frame, frames_between_screenshots;
  bool record_movie;
  float bcolor[3]; //Background color
  //State control variables-----------
  bool dostep;
  bool pause;
  bool running;
  unsigned int frame_counter, shot_counter, frame_shot_counter;
  bool record;
  //-----------------------
};

void App::init_default_params(){
  bcolor[0] = bcolor[1] = bcolor[2] = 0.0;
  record_movie = false;
  updates_per_frame = 0;
  sf::Mouse::setPosition(sf::Vector2i(FWIDTH/2, FHEIGHT/2));
  shot_counter = frame_shot_counter = 0;
  record = false;
  frame_counter = 0;
  frames_between_screenshots = 0;
  pause = true;
  dostep = false;
  running = true;
}

void App::parse_input(int argc, char** argv){
  fori(0,argc){
    if(argc==1 || strcmp(argv[i],"-h")==0){
      if(argc==1) cout<<"\nERROR!!! MISSING FILENAME"<<endl;
      print_help();
      exit(0);
    }
    if(argv[i][0]!='-') continue;
    if(strcmp(argv[i],"--record")==0) record_movie = true;
    if(strcmp(argv[i],"--palette")==0) palette_id = atoi(argv[i+1]);
    if(strcmp(argv[i],"--frames-between-screenshots")==0)frames_between_screenshots = atoi(argv[i+1]);
    if(strcmp(argv[i],"--background")==0){
      bcolor[0] = stod( argv[i+1]);
      bcolor[1] = stod( argv[i+2]);
      bcolor[2] = stod( argv[i+3]);
    }

  }
}

App::App(int argc, char** argv){
  cout<<"Welcome to Superpunto v1.0!\n";
  cout<<"INIT...\n";
  glewExperimental = GL_TRUE;

  this->init_default_params();
  this->parse_input(argc,argv);
 
  glewInit();
  ContextSettings context(24, 8, 0, 3, 0);
  window.create(VideoMode(FWIDTH,FHEIGHT), "Superpunto",Style::Default, context);
  float GLVER = (float)window.getSettings().majorVersion+ 0.1f*window.getSettings().minorVersion;
  printf("OpenGL Version available: %1.1f\n", GLVER);

  if(!checksystem(GLVER)){
    cout<<"Invalid OpenGL Version!!, min 3.0 needed!, 3.3+ recommended"<<endl;
    cout<<"Unexpected behavior can occur!!"<<endl;
  }
  
  /*if(GLVER>3.3)*/ window.setVerticalSyncEnabled(true);

  glClearColor(bcolor[0], bcolor[1], bcolor[2], 1.0f);   
  glcontext.initialize(); 

  font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeSans.ttf");
  text.setFont(font); 
  text.setCharacterSize(24);
  text.setColor(sf::Color(255*(1-bcolor[0]), 255*(1-bcolor[1]), 255*(1-bcolor[2])));

  cout<<"DONE!"<<endl;
}
void App::handle_events(){
  Event event;
  while (window.pollEvent(event)){
    glcontext.handle_event(event);
    if (event.type == Event::Closed)running=false;
    if (event.type == Event::Resized){
      float w = event.size.width;
      float h = event.size.height;
      glViewport(0.0, 0.0, w, h);
      glcontext.change_aspect(w/h);
      window.setView(sf::View(sf::FloatRect(0.f, 0.f, w, h)));
    }
    if (event.type == Event::KeyPressed){
      IF_KEY(Escape, running=false;)
      IF_KEY(N, dostep = true; pause = !pause;)
      IF_KEY(L, record = !record; glcontext.play_movie = true;)
      IF_KEY(C, screenshot();)
      IF_KEY(LAlt, sf::Mouse::setPosition(sf::Vector2i(FWIDTH/2, FHEIGHT/2));)
    }
    if(event.type == Event::KeyReleased){
      IF_KEY(H, print_help();)
    }
  }
}
void App::Run(){
  if(record_movie){
    printf("Recording...\n");
    window.force_draw();
    fori(0,10)draw();
    record = true;
    glcontext.play_movie = true;
    while(glcontext.current_step<glcontext.Nframes){
      cout<<glcontext.current_step<<" "<<glcontext.Nframes<<endl;
      window.force_draw();
      draw();
    }
    window.close();
    return;
  }
 
  while(window.isOpen()){ 
    draw();
    if(!running)window.close();
  }
}
void App::draw(){
  if(window.ready_to_draw()){
    handle_events();
    window.update_fps("Superpunto v1.0 stable! -- "+to_string(glcontext.Nframes)+" frames loaded -- ");
    glcontext.update();
    glcontext.draw();
    frame_counter++;
    if(record)if(frame_counter%(frames_between_screenshots+1)==0) this->record_frame();
    
    text.setString(glcontext.current_msg()+" -- "+to_string(glcontext.current_step));
    window.pushGLStates();
    window.draw(text);
    window.popGLStates();
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
  if(argc>1)  fileName = std::string(argv[1]);
  App app(argc,argv);
  app.Run();
  app.Exit();
  printf("Properly exiting...DONE!\nHasta la vista!\n");
 return 0;
}
