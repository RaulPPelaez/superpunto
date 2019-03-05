#ifndef RFILE_H
#define RFILE_H

#include"header.h"
#include<vector>
#include<string>
#include<fstream>
#include<stdio.h>
#include<sstream>
#include<iostream>
using namespace std;
#include"math_helper.h"





typedef unsigned int uint;




class Superbin{  
public:
  enum SMODE {SREAD, SWRITE};
  struct FileConfig2{
    unsigned int nrows;
    unsigned int nframes;
    unsigned int maxN;
    std::vector<int> N;
  };


  struct FileConfig{
    unsigned int nframes;
    unsigned int nrows;
  };

  
  Superbin(string fileName){
    f.open(fileName, ios::in | ios::binary);
    mode = SREAD;
    this->read_header();    
  }
  Superbin(string fileName, int nframes, int nrows){
    finfo.nframes = nframes;
    finfo.nrows = nrows;
    f.open(fileName, ios::out | ios::binary);
    mode = SWRITE;
    this->print_header();
  }
  ~Superbin(){ f.close();}

  void close(){f.close();}
  void print_frame(const char *msg, const float *data, int Np){
    string str(msg);
    print_frame(str, data, Np);
  }
  void print_frame(const string &msg, const float *data, int Np){
    if(mode == SREAD){cout<<"INVALID USE OF WRITE OBJECT!!!!"<<endl; return;}
    int msize = msg.size();
    f.write((char *)&msize, sizeof(int));
    f.write(msg.c_str(), msize+1);
    f.write((char *)&Np, sizeof(int));
    f.write((char *)&data[0], Np*finfo.nrows*sizeof(float));

  }
  void read_frame(string &comment, vector<float> &data){
    if(mode == SWRITE){cout<<"INVALID USE OF READ OBJECT!!!!"<<endl; return;}

    int msize;
    f.read((char *)&msize, sizeof(int));
    char buf[msize+1];
    f.read(buf, msize+1);
    comment = string(buf);
    int Np;
    f.read((char *)&Np, sizeof(int));
    data.resize(Np*finfo.nrows);
    f.read((char *)&data[0], Np*finfo.nrows*sizeof(float));
  }
  
  FileConfig get_FileConfig(){
    return this->finfo;
  }
  
private:
  void print_header(){
    f.write((char *)&finfo.nframes, sizeof(int));
    f.write((char *)&finfo.nrows, sizeof(int));
  }
  void read_header(){
    f.read((char *)&finfo.nframes, sizeof(int));
    f.read((char *)&finfo.nrows, sizeof(int));    
  }
  fstream f;
  FileConfig finfo;
  SMODE mode;
};







enum ColorParserType{PALETTE, RANGE, HEXBGR, DEFAULT};

struct RColor{
  float r,g,b;
};


class ColorParser{
public:
  ColorParser(ColorParserType tp, uint pid=923302100);

  RColor getColor(uint id);
   
  uint palette_id;//=923302100; //1 is also cool


private:
  RColor id2BGR(uint id);
  ColorParserType tp;
  vector<uint> palette;

};



struct ParticleData{
  float *pos=nullptr, *scales=nullptr, *colors=nullptr;
  float *vel=nullptr; //for arrows
  uint N=0;
  float3 L;
};

struct RFile{
  const char *name;
  uint Nrows;
  uint Nframes;
  uint maxN;
  uint current_frame;

  vector<int> Natframe;
  
  vector<vector<float>> pos, vel, scales, colors;
  vector<float3> Lbox;
  vector<float3> max_dist;
  vector<string> msgs;
  float maxScale;

  ParticleData current;
  
  bool set_frame(int frame);
  bool set_next_frame();
  bool set_previous_frame();

  bool get_config();
  bool iscomment(std::string line);
  bool read_frames(ColorParserType read_color_mode);
  bool read_frames_binary(ColorParserType read_color_mode);
  void parse_comment(std::string line, std::string &msg, float *L);
};


enum RenderType{PARTICLES, ARROWS, NONE};

struct RConfig{
  bool record_movie;
  bool play;
  int frames_between_screenshots;
  float bcolor[3];
  std::string fontName;

  RenderType render_type;
  
  //bool readHexColor;
  ColorParserType read_color_mode;
  bool binary_read_mode;
  bool noaxis, nobox;
  int FW, FH; //input Resolution
  
  bool parse_args(int argc, char *argv[]);
  void print_help();
  void set_default();
};

string read_file(const char *fileName);


#endif
