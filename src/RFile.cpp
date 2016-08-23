#include"RFile.h"
#include"math_helper.h"



bool RFile::set_frame(int frame){
  if(frame>Nframes-1  || frame<0) return false;
  current_frame = frame;
  current.pos = pos[frame].data();
  current.scales = scales[frame].data();
  current.colors = colors[frame].data();
  current.N = Natframe[frame];
  if(Lbox[frame].y == 0 && Lbox[frame].x == 0 && Lbox[frame].z == 0)
    current.L = max_dist[frame]*2.0f+2*maxScale;
  else
    current.L = Lbox[frame]*2.0f;
  return true;
}
bool RFile::set_next_frame(){
  if(current_frame<(Nframes-1))current_frame++;
  else return false;
  set_frame(current_frame);
  
  return true;
}
bool RFile::set_previous_frame(){
  if(current_frame>0)current_frame--;
  else return false;
  set_frame(current_frame);
  return true;
}

bool RFile::iscomment(std::string line){
  char iscomment = line[line.find_first_not_of(" \t\r\n")];
  return iscomment=='#';
}

bool RFile::get_config(){
  ifstream in(name);
  if(!in) return false;
  printf("Getting file config from %s...\n", name);

  Nframes = 1; Nrows = 0;
  string line;
  /*Skip comments*/
  do{ getline(in,line); }while(iscomment(line));
  
  /*Count rows in the first line*/
  float trash;
  stringstream is(line);
  while (is >> trash) Nrows++;
  unsigned int N= 0;
  maxN = 1;

  while(!in.eof()){
    if(iscomment(line)){
      Nframes++;
      Natframe.push_back(N);
      if(N>maxN){maxN = N;}
      N = 0;
      do{
	getline(in,line);
	if(in.eof()){Nframes--;break;}
      }while(iscomment(line));
    }
    N++;
    getline(in, line);
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
  
  maxScale = 0.0f;  
  ifstream in(name);   
  /*Reserve space for all the file*/
  pos.resize(Nframes);
  scales.resize(Nframes);
  colors.resize(Nframes);
  msgs.resize(Nframes," ");
  Lbox.resize(Nframes, make_float3(0.0f));
  max_dist.resize(Nframes, make_float3(0.0f));
  
  /*Helper variables*/
  std::string line;  
  std::stringstream is;

  float temp[Nrows];
  vector<int> ctemp(Natframe[0],0);
  
  /*Generate color pallete*/
  srand(palette_id);
  vector<unsigned int> palette(1000,0);
  fori(0,1000) palette[i] = rand()%0xffFFff;

  /*Read all the frames*/
  getline(in,line);
  for(int frame = 0; frame<Nframes; frame++){
    printf("\rLoading data... %d%%   ",
	   (int)(100.0f*(float)frame/(float) Nframes +0.5f));
    fflush(stdout);

    int N = Natframe[frame];
    /*Reserve space*/
    pos[frame].resize(3*N,0);
    scales[frame].resize( N,1);
    //colors[frame].resize(3*N,1);    //No need for this
    ctemp.resize(N);
    /*Read and parse comments*/
    while(iscomment(line)){
      parse_comment(line, msgs[frame], (float*)&(Lbox[frame]));
      getline(in,line);
    }
    /*Read particle data*/
    fori(0, N){
      is.clear();
      is.str(line);
      temp[3] = 0.0f; //Radius
      temp[4] = 1.0f; //Color

      /*Read all the rows, XYZ minimum*/
      forj(0, Nrows){
	is>>temp[j];
      }
      if(Nrows==4) swap(temp[3], temp[4]);
      /*Get color id from palette*/
      ctemp[i] = palette[((int)temp[4]+1)%palette.size()];
      /*Save data*/
      forj(0,3){
      pos[frame][3*i+j] = temp[j];
      ((float*)&max_dist[frame])[j] =
	Rmax(((float*)&max_dist[frame])[j], temp[0]);
      }
      
      scales[frame][i] = temp[3];
      /*Keep record of the biggest particle*/
      if(frame==0) if(temp[3]>maxScale) maxScale = temp[3];
      
      getline(in,line);
    }
    /*Decode color ids*/
    colors[frame] = parse_colors(ctemp);
    
  }
  printf("\rLoading data... 100 %%   \nDONE!\n");
  
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

float get_flag(std::string flag, std::string &line){
  float res = 0.0f;
  size_t pos = 0;
  std::string token;
  pos = line.find(flag);
  if(pos != std::string::npos){
    line.erase(0, pos + flag.length());
    pos = line.find(std::string(";"));
    token = line.substr(0,pos);
    res = std::stof(token, NULL);
    line.erase(0, pos+1);
  }
  return res;
}

void RFile::parse_comment(std::string line, std::string &msg, float *L){
  static vector<std::string> delims;
  static std::string delim2 = ";";
  static bool init = false;
  if(!init){
    delims.push_back("Lx=");
    delims.push_back("Ly=");
    delims.push_back("Lz=");
    init = true;
  }

  fori(0,3){
    L[i] = get_flag(delims[i], line);
  }
  float Lt = get_flag(std::string("L="), line);
  if(Lt!=0.0f) L[0] = L[1] = L[2] = Lt;
  
  msg = line;
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
    if(strcmp(argv[i],"--use-font")==0) fontName = std::string(argv[i+1]);
    
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
  fontName  = std::string("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf");
}



string read_file(const char *fileName){
  std::ifstream in(fileName);
  std::string contents((std::istreambuf_iterator<char>(in)), 
		       std::istreambuf_iterator<char>());
  return contents;
}

