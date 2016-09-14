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
  case 2:case 1:
     printf("\t INVALID NUMBER OF ROWS IN FILE!\n");
     exit(0);
     break;
  default:
    printf("\t%d rows, taking as XYZRC\n\t%d frames\n\t%d superpuntos\n", fc.nrows, nframes, maxN);
    fc.nrows = 5;
     break;
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
void print_help(){

      printf("\n\x1b[1m Usage:  spunto file [opts]  \x1b[0m\n\n");
      printf("\x1b[1m  Options:\x1b[0m\n");
      printf("\t  --record :  Makes a movie of all the frames in file and generates a .gif\n");
      printf("\t  --frames-between-screenshots X : Number of frames skipped between screenshots when recording (default = 2)\n");
      printf("\t  --background R G B : Background color in RGB, default R=G=B=0.0\n");
      printf("\t  --palette X : Change the color palette\n");
      printf("\t  --RGB : Interpret color column as RGB color id instead of palette generated color (as integers, 255 is red)\n");

      printf("\n\x1b[1m Controls:\x1b[0m\n");
      printf("  Movement:\n");
      printf("\t Move with WASD, E and Q to tilt and Shift/Ctrl to go up/Down\n");
      printf("\t Use +/- to increase/decrease the speed\n");
      printf("\t Look around holding ALT and moving the mouse\n");
      printf("\t Rotate the world in XYZ using 123/456\n");
      printf("  Frame control:\n");
      printf("\t Press Space to go to the next frame, R to the previous\n");
      printf("\t Press T to go to the last frame, B takes you to the first one\n");
      printf("\t Press M to play the frames at 15 FPS, M again to pause\n");
      printf("\t Press C to take a screenshot in png\n\t Press L to play and record to a gif until L is pressed again\n");
      printf("  Others:\n");
      printf("\t Press h to print this help page\n");
}
