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

struct RFile{
  const char *name;
  uint Nrows;
  uint Nframes;
  uint maxN;
  uint current_frame;

  vector<int> Natframe;
  
  vector<vector<float>> pos, scales, colors;
  vector<float3> Lbox;
  vector<float3> max_dist;
  vector<string> msgs;
  float maxScale;
  
  bool get_frame(float *&ps, float *&cs, float *&ss, int &N, int frame);
  bool get_next_frame(float *&ps, float *&cs, float *&ss, int &N);
  bool get_previous_frame(float *&ps, float *&cs, float *&ss, int &N);


  bool get_config();
  bool iscomment(std::string line);
  bool read_frames();
  vector<float> parse_colors(const std::vector<int> &colors);
  void parse_comment(std::string line, std::string &msg, float *L);
};


struct RConfig{
  bool record_movie;
  bool play;
  int frames_between_screenshots;
  float bcolor[3];
  
  bool parse_args(int argc, char *argv[]);
  void print_help();
  void set_default();
};

string read_file(const char *fileName);


#endif
