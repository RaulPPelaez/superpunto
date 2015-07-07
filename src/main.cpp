#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<fstream>
#include<string>
#include<sstream>
#include<cmath> 

#include<vector>

#include "defines.h"
#include "utils.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <SFML/Graphics.hpp>
using namespace sf;

#include <stdlib.h>


#include"glib.h"
#include "RModelHandler.h"
#include "Camera.h"
#include "../tools/gif-h/gif.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;


std::string fileName;

class RWindow: public sf::RenderWindow{
  public:
    RWindow(){
     frames=0;
     FPS=TARGET_FPS;
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
    bool ready_to_draw(){
      float time = draw_clock.getElapsedTime().asMicroseconds();
      if(time>=(1e6/TARGET_FPS)){
	draw_clock.restart();
	   
	return true;
      }
      return false;
    }
  private:
    int frames;
    float FPS;
    Clock fps_clock;
    Clock draw_clock;
};

 
class RGL{
  public:
    RGL():cam(){}
    ~RGL(){
      glDeleteBuffers(3,instancing_vbos);
    }
    void initialize();
    void draw();
	
    void update();
    
    void handle_event(sf::Event event);
    
    void rotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
    void scale(GLfloat x, GLfloat y, GLfloat z);
    void translate(GLfloat x, GLfloat y, GLfloat z);

    void lookAt(const glm::vec3 &pos, const glm::vec3 &view, const glm::vec3 &up);

    void loadIdentity();
    void upload_MVP();
    bool play_movie;
  private:	
    //GLuint positions_vbo;
    void loadData();
    void upload_step();
    glm::mat4 proj, model, view, MVP;
    FreeCamera cam;
	    
    glm::vec3 lightpos;
    glm::vec3 modle_angles;
    
    
    float frames;
    
    int Nframes;
    RModelHandler drawables;
    std::vector<vector<float>> positions, colors,scales;
    GLuint instancing_vbos[3];
    int current_step;    
	    
};

void RGL::initialize(){  
  srand(time(NULL));
  frames = 0.0;

 drawables.initialize(&MVP, &model);
  
 drawables.add_models();  
 
 loadData();
 
    
  glEnable(GL_DEPTH_TEST);

  proj = glm::perspective(45.0f, 1.0f, 0.01f, 10000.0f);
  //lookAt(cam.pos, cam.get_view(), cam.up); 
  lightpos = glm::vec3(0,0,-1);
  play_movie = false;
  loadIdentity();
  printf("DONE!\nDrawing...\n");

}

void RGL::draw(){
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    this->view = cam.lookAt();
    drawables.get_pr()->use();
    drawables.config_light();
    glUniform3f(glGetUniformLocation(drawables.get_pr()->id(), "EyeWorldPos"), cam.pos.x, cam.pos.y, cam.pos.z);
    glUniform3f(glGetUniformLocation(drawables.get_pr()->id(),"point_light.Position"), lightpos.x, lightpos.y, lightpos.z);
	
    upload_MVP();
    drawables.draw_model(0);
    

    
}

void RGL::upload_MVP(){
    MVP = proj*view*model;
    //glUniformMatrix4fv(uniMVP , 1, GL_FALSE, &MVP[0][0]);
    //glUniformMatrix4fv(unimodel , 1, GL_FALSE, &model[0][0]);
}
void RGL::handle_event(sf::Event event){
 if (event.type == Event::KeyPressed){
   IF_KEY(Up, lightpos.z += 0.5;)   
   IF_KEY(Down, lightpos.z -= 0.5;)
   IF_KEY(Space, upload_step();)
   
   IF_KEY(R, current_step= Rmax(current_step-2,0);upload_step();)

   IF_KEY(M, play_movie = !play_movie;)

   IF_KEY(Num4, rotate(0.1f,1,0,0);)
   IF_KEY(Num5, rotate(0.1f,0,1,0);)
   IF_KEY(Num6, rotate(0.1f,0,0,1);)
   
   IF_KEY(Num1, rotate(-0.1f,1,0,0);)
   IF_KEY(Num2, rotate(-0.1f,0,1,0);)
   IF_KEY(Num3, rotate(-0.1f,0,0,1);)
 }
  
}
void RGL::rotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z){
    model = glm::rotate(model, angle,  glm::vec3(x, y, z));
}
void RGL::scale(GLfloat x, GLfloat y, GLfloat z){
    model = glm::scale(model, glm::vec3(x, y, z));
}
void RGL::translate(GLfloat x, GLfloat y, GLfloat z){
    model = glm::translate(model, glm::vec3(x, y, z));
}

void RGL::lookAt(const glm::vec3 &pos, const glm::vec3 &view, const glm::vec3 &up){
	this->view = glm::lookAt(pos,view,up);
	//glUniformMatrix4fv(uniview , 1, GL_FALSE, glm::value_ptr(this->view));
}
void RGL::loadIdentity(){ 
model = glm::mat4();
}


	
void RGL::update(){
	this->cam.update(); 
	if(play_movie)upload_step();

}

void RGL::loadData(){
  
  ifstream in(fileName.c_str());
  FileConfig fc = get_config(fileName.c_str());
    
  std::string line;  
  
  int c;
  int Nmax=fc.maxN;
  Nframes = fc.nframes;
  positions.resize(fc.nframes);
  scales.resize(fc.nframes);
  colors.resize(fc.nframes);
  
  positions[0].resize(3*fc.N[0],0);
  scales[0].resize(fc.N[0],1);
  colors[0].resize(3*fc.N[0],1);    
  
  getline(in,line);
  if(line.substr(0,1) == "#") getline(in,line);
  std::stringstream is(line);
  
  double temp[fc.nrows];
  vector<int> ctemp(fc.N[0],0);
  int frame = 0;
  int N = -1;
  printf("\rLoading data... %d%%   ", 0);
  fflush(stdout);
  while(!in.eof()){
   if(line.substr(0,1)!="#") N++;
   else{
     colors[frame] = parse_colors(ctemp);
     N=-1; frame++; 
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
   
   fori(0,3)positions[frame][3*N+i] = temp[i];
   
   scales[frame][N] = temp[3];
   ctemp[N] = temp[4]*39275;
   
   getline(in,line);
  }
  printf("\rLoading data... 100 %%   \nDONE!\n\nSetting up graphic enviroment...");
  colors[frame] = parse_colors(ctemp);

  
  glGenBuffers(3, instancing_vbos);
  
  drawables.set_instancing("color",0, instancing_vbos[1], 0, 3, 3*sizeof(float));
  drawables.set_instancing("scale",0, instancing_vbos[2], 0, 1, sizeof(float));
  drawables.set_instancing("pos",0, instancing_vbos[0], positions[0].size()/3, 3, 3*sizeof(float));

  current_step = 0;
  upload_step();

}

void RGL::upload_step(){
  int frame = current_step;
  if(current_step<Nframes-1)current_step++;

  glBindBuffer(GL_ARRAY_BUFFER, instancing_vbos[2]);
  glBufferData(GL_ARRAY_BUFFER, scales[frame].size()*sizeof(float), scales[frame].data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, instancing_vbos[1]);
  glBufferData(GL_ARRAY_BUFFER, colors[frame].size()*sizeof(float), colors[frame].data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, instancing_vbos[0]);
  glBufferData(GL_ARRAY_BUFFER, positions[frame].size()*sizeof(float), positions[frame].data(), GL_DYNAMIC_DRAW);
  
  glBindBuffer(GL_ARRAY_BUFFER,0);
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
    RGL glcontext;
    unsigned int shot_counter, frame_shot_counter;
    bool record;
    unsigned int frame_counter;
    std::vector<sf::Image> GIF;
    GifWriter GIF2;
};

App::App(int argc, char** argv){
 //glewExperimental = GL_TRUE;
 glewInit();
  ContextSettings context(24, 8, 2, 0, 30);
  window.create(VideoMode(FWIDTH,FHEIGHT), "Superpunto",Style::Default, context);
  //window.setPosition(Vector2i(0,0));
  
  window.setActive(true);
  
  if(TARGET_FPS==60) window.setVerticalSyncEnabled(true);

  glClearColor(.5, .5, .5, 1.0f);   
 
  glcontext.initialize();
  
  sf::Mouse::setPosition(sf::Vector2i(FWIDTH/2, FHEIGHT/2));
  shot_counter = frame_shot_counter = 0;
  record = false;
  frame_counter = 0;
  

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
      IF_KEY(L, record = !record; glcontext.play_movie = true;)
      IF_KEY(C, screenshot();)
       
    }
  }   
}
void App::Run(){
 
  while(window.isOpen()){ 
    draw();
  }
}
void App::draw(){
  if(window.ready_to_draw()){
    window.update_fps();
    glcontext.update();
    glcontext.draw();
    frame_counter++;
    if(record)if(frame_counter%2==0) this->record_frame();
    window.display();
    handle_events();
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
  if(frame_shot_counter>1){
    printf("\n\nConverting to .gif...");
    fflush(stdout);
    GifEnd(&GIF2);
    cout<<"DONE!"<<endl;
  }
}
int main(int argc, char** argv){
  fileName = std::string(argv[1]);
  App app(argc,argv);
  app.Run();
  app.Exit();
  printf("Properly exiting...DONE!\nHasta la vista!\n");
 return 0;
}
