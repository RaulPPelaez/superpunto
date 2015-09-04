#include "utils.h"



FileConfig get_config(const char *fileName){
  printf("Getting file config...\n");
  FileConfig fc;
  std::ifstream in(fileName);
  int nframes = 1, nrows = 0;
  std::string line;
  getline(in,line);
  if(iscomment(line)) getline(in,line);
  std::vector<int> Natframes;
  double trash;
  std::stringstream is(line);
  while (is >> trash) nrows++;
  unsigned int N= 0, maxN = 1;
  while(!in.eof()) {
    if(iscomment(line)){
      nframes++;
      Natframes.push_back(N);
      if(N>maxN){maxN = N;}
      N = -1;
    }
    N++;
    getline (in, line);
  }
  if(N>maxN){maxN = N;}
  Natframes.push_back(N);
  fc.nrows = nrows;
  fc.nframes = nframes;
  fc.maxN = maxN;
  fc.N = Natframes;
  
  printf("\t%d rows, taking as XYZRC\n\t%d frames\n\t%d superpuntos\n", nrows, nframes, maxN);
  
  return fc;  
}



bool iscomment(std::string line){
  std::stringstream s(line);
  char iscomment;
  s>>iscomment;
  return iscomment=='#';
}
