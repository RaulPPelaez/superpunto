#include"RFile.h"

bool RFile::get_frame(float *&ps, float *&cs, float *&ss, int &N, int frame){
  if(frame>Nframes-1  || frame<0) return false;
  ps = pos[frame].data();

  cs = colors[frame].data();
  ss = scales[frame].data();
  //  printf("%.3f %.3f %.3f %.3f\n", ps[3], ps[4], ps[5], ss[1]);
  N = Natframe[frame];
  current_frame = frame;
  return true;
}
bool RFile::get_next_frame(float *&ps, float *&cs, float *&ss, int& N){
  if(current_frame<(Nframes-1))current_frame++;
  else return false;
  get_frame(ps,cs,ss, N, current_frame);
  
  return true;
}
bool RFile::get_previous_frame(float *&ps, float *&cs, float *&ss, int &N){
  if(current_frame>0)current_frame--;
  else return false;
  get_frame(ps,cs,ss, N, current_frame);
  return true;
}

bool RFile::iscomment(std::string line){
  line.erase(0, line.find_first_not_of(" \t\r\n"));
  char iscomment = line[0];
  return iscomment=='#';
}

bool RFile::get_config(){
  ifstream in(name);
  if(!in) return false;
  printf("Getting file config from %s...\n", name);

  Nframes = 1; Nrows = 0;
  string line;
  getline(in,line);
  if(iscomment(line)) getline(in,line);
  double trash;
  stringstream is(line);
  while (is >> trash) Nrows++;
  unsigned int N= 0;
  maxN = 1;

  while(!in.eof()) {
    if(iscomment(line)){
      Nframes++;
      Natframe.push_back(N);
      if(N>maxN){maxN = N;}
      N = -1;
    }
    N++;
    getline (in, line);
  }
  if(N>maxN){maxN = N;}
  Natframe.push_back(N);
  switch(Nrows){
   case 5:
     printf("\t5 rows, taking as XYZRC\n\t%d frames\n\t%d superpuntos\n", Nframes, maxN);
     break;
   case 4:
     printf("\t4 rows, taking as XYZC\n\t%d frames\n\t%d superpuntos\n", Nframes, maxN);
     break;
   case 3:
     printf("\t3 rows, taking as XYZ\n\t%d frames\n\t%d superpuntos\n", Nframes, maxN);
     break;
  case 2:case 1:
     printf("\t INVALID NUMBER OF ROWS IN FILE!\n");
     exit(0);
     break;
  default:
    printf("\t%d rows, taking as XYZRC\n\t%d frames\n\t%d superpuntos\n", Nrows, Nframes, maxN);
    Nrows = 5;
     break;
  }
  in.close();
  return true;
}

bool RFile::read_frames(){
  current_frame = 0;
  //string file = read_file(name);
  ifstream in(name);   
  //istringstream in(file);
  int c;
  pos.resize(Nframes);
  scales.resize(Nframes);
  colors.resize(Nframes);
  msgs.resize(Nframes," ");
  Lbox.resize(Nframes);
  max_dist.resize(Nframes, 0);
  
  pos[0].resize(3*Natframe[0],0);
  scales[0].resize( Natframe[0],1);
  colors[0].resize(3*Natframe[0],1);    

  std::string line;  

  getline(in, line);
  if(iscomment(line)){Lbox[0] = parse_comment(line, msgs[0]); getline(in,line);}

  std::stringstream is(line);

  vector<double> temp(Nrows);
  vector<int> ctemp(Natframe[0],0);
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
     pos[frame].resize(3*Natframe[frame],0);
     scales[frame].resize(Natframe[frame],1);
     ctemp.resize(Natframe[frame],1);
     getline(in,line);
     printf("\rLoading data... %d%%   ", (int)(100.0f*(float)frame/(float) Nframes +0.5f));
     fflush(stdout);
     continue;
   }
   is.clear();
   is.str(line);
   for(int i=0; i< Nrows;i++) is>>temp[i];
   if( Nrows==3){temp[3] = 1; temp[4] = 1;}
   if( Nrows==4){temp[4] = temp[3]; temp[3]=1;}
   fori(0,3){
     pos[frame][3*N+i] = temp[i];
     max_dist[frame] = max(max_dist[frame], pos[frame][3*N+i]);
   }

   scales[frame][N] = temp[3];
   ctemp[N] = palette[((int)temp[4]+1)%1000];
   
   getline(in,line);
  }
  printf("\rLoading data... 100 %%   \nDONE!\n");
  colors[frame] = parse_colors(ctemp);
  return true;
}

vector<float> RFile::parse_colors(const std::vector<int> &colors){
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
float RFile::parse_comment(std::string line, std::string &msg){
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


bool RConfig::parse_args(int argc, char *argv[]){
  fori(0,argc){
    if(argc==1 || strcmp(argv[i],"-h")==0){
      if(argc==1) printf("\nERROR!!! MISSING FILENAME\n");
      print_help();
      exit(0);
    }
    if(argv[i][0]!='-') continue;
    if(strcmp(argv[i],"--record")==0) record_movie = true;
    if(strcmp(argv[i],"--palette")==0) palette_id = atoi(argv[i+1]);
    if(strcmp(argv[i],"--frames-between-screenshots")==0) frames_between_screenshots = atoi(argv[i+1]);
    if(strcmp(argv[i],"--background")==0){
      bcolor[0] = stod( argv[i+1]);
      bcolor[1] = stod( argv[i+2]);
      bcolor[2] = stod( argv[i+3]);
    }
  }
  return true;
}

void RConfig::print_help(){
  printf("\n\x1b[1m Usage:  spunto file [opts]  \x1b[0m\n\n");
  printf("\x1b[1m  Options:\x1b[0m\n");
  printf("\t  --record :  Makes a movie of all the frames in file and generates a .gif\n");
  printf("\t  --frames-between-screenshots X : Number of frames skipped between screenshots when recording (default = 2)\n");
  printf("\t  --background R G B : Background color in RGB, default R=G=B=0.0\n");
  printf("\t  --palette X : Change the color palette\n");

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


void RConfig::set_default(){
  record_movie = false;
  play = false;
  frames_between_screenshots = 2;
  bcolor[0] = bcolor[1] = bcolor[2] = 0.0f;
}



string read_file(const char *fileName){
  std::ifstream in(fileName);
  std::string contents((std::istreambuf_iterator<char>(in)), 
		       std::istreambuf_iterator<char>());
  return contents;
}

