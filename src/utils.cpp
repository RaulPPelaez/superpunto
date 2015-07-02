#include "utils.h"



FileConfig get_config(const char *fileName){
  printf("Getting file config...\n");
  FileConfig fc;
  std::ifstream in(fileName);
  int nframes = 1, nrows = 0;
  std::string line;
  getline(in,line);
  if(line.substr(0,1) == "#") getline(in,line);
  std::vector<int> Natframes;
  double trash;
  std::stringstream is(line);
  while (is >> trash) nrows++;
  unsigned int N= 0, maxN = 1;
  while(!in.eof()) {
    if(line.substr(0,1)=="#"){
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




std::vector<float> parse_colors(const std::vector<int> &colors){
  
 int N = colors.size();
 std::vector<float> RGB(3*N);
 
 for(int i=0; i<3*N; i+=3){
   int c = colors[i/3];
   RGB[i] = (c & 0xFF)/255.0f;
   RGB[i+1] = ((c & 0xFF00) >> 8)/255.0f;
   RGB[i+2] = ((c & 0xFF0000) >> 16)/255.0f;
 }
 return RGB;   
  
}