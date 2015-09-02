#ifndef UTILS_H
#define UTILS_H

#include<fstream>
#include<string>
#include<sstream>
#include<vector>

struct FileConfig{
  unsigned int nrows;
  unsigned int nframes;
  unsigned int maxN;
  std::vector<int> N;
};

FileConfig get_config(const char *fileName);





#endif
