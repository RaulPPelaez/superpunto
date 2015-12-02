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
  switch(nrows){
   case 5:
     printf("\t5 rows, taking as XYZRC\n\t%d frames\n\t%d superpuntos\n", nframes, maxN);
     break;
   case 4:
     printf("\t4 rows, taking as XYZC\n\t%d frames\n\t%d superpuntos\n", nframes, maxN);
     break;
   case 3:
     printf("\t3 rows, taking as XYZ\n\t%d frames\n\t%d superpuntos\n", nframes, maxN);
     break;
   default:
     printf("\t INVALID NUMBER OF ROWS IN FILE!\n");
     exit(0);
  }
  in.close();
  return fc;  
}



bool iscomment(std::string line){
  line.erase(0, line.find_first_not_of(" \t\r\n"));
  char iscomment = line[0];
  return iscomment=='#';
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
      
bool checksystem(float ver){
  if(ver<3.0) return false;
  else return true;

}
