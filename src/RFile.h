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
  
  bool parse_args(int argc, char *argv[]);
  void print_help();
  void set_default();
};

string read_file(const char *fileName);


#endif
