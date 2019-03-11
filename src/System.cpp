#include"System.h"

#include<unistd.h>
#include<fstream>
namespace superpunto{
  void System::parseArguments(){
    options.readFile = "/dev/stdin";

    //Look for the file, in stdin or in a given filename
    if(isatty(STDIN_FILENO)){ //If there is no pipe
      bool good_file = false;
      fori(1,m_argc){
	auto in = std::ifstream(m_argv[i]); //There must be a filename somewhere in the cli
	if(in.good()){
	  good_file = true;
	  options.readFile = m_argv[i];
	  break;
	}
      }
      //If no valid file was found
      if(!good_file){
	printHelp();
	log<System::CRITICAL>("[System] ERROR! No valid input provided");

      }
    }
    
    fori(0,m_argc){
      if(strcmp(m_argv[i] , "-h")==0){
	printHelp();
	exit(0);
      }      
      if(m_argv[i][0]!='-') continue;
      if(strcmp(m_argv[i],"--record")==0) options.record_movie = true;

      if(strcmp(m_argv[i],"--palette")==0) options.palette_id = atoi(m_argv[i+1]);
      if(strcmp(m_argv[i],"--frames-between-screenshots")==0) options.frames_between_screenshots = atoi(m_argv[i+1]);
      if(strcmp(m_argv[i],"--RGB")==0) options.read_color_mode=ColorParserType::HEXBGR;//readHexColor = true;
      if(strcmp(m_argv[i],"--renderer")==0){
	if(strcmp(m_argv[i+1],"arrows")==0)
	  options.render_type = RenderType::ARROWS;
	if(strcmp(m_argv[i+1],"particles")==0)
	  options.render_type = RenderType::PARTICLES;
      }
      if(strcmp(m_argv[i],"--background")==0){
	options.bcolor[0] = std::stod( m_argv[i+1]);
	options.bcolor[1] = std::stod( m_argv[i+2]);
	options.bcolor[2] = std::stod( m_argv[i+3]);
      }
      if(strcmp(m_argv[i],"--resolution")==0){
	options.target_FW = std::atoi( m_argv[i+1]);
	options.target_FH = std::atoi( m_argv[i+2]);      
      }
      if(strcmp(m_argv[i],"--binary")==0) options.binary_read_mode = true;
      if(strcmp(m_argv[i],"--noaxis")==0) options.noaxis = true;
      if(strcmp(m_argv[i],"--nobox")==0) options.nobox = true;
      if(strcmp(m_argv[i],"--use-font")==0) options.fontName = std::string(m_argv[i+1]);
    
    }
  }

  void System::printHelp(){
    log<System::MESSAGE>("\x1b[1m Usage:  spunto file [opts]  \x1b[0m");
    log<System::MESSAGE>("\x1b[1m  Options:\x1b[0m");
    log<System::MESSAGE>("\t  --record :  Makes a movie of all the frames in file and generates a .mp4");
    log<System::MESSAGE>("\t  --frames-between-screenshots X : Number of frames skipped between screenshots when recording (default = 2)");
    log<System::MESSAGE>("\t  --background R G B : Background color in RGB, default R=G=B=0.0");
    log<System::MESSAGE>("\t  --palette X : Change the color palette");
    log<System::MESSAGE>("\t  --RGB : Read colors as hex values in BGR (as integers) (0xFF=red=255). Overrides palette");
    //log<System::MESSAGE>("\t  --renderer [render=arrows,particles]: Rendering mode.");
    log<System::MESSAGE>("\t  --nobox : Do not render the bounding box.");
    log<System::MESSAGE>("\t  --noaxis : Do not render axis labels.");
    //log<System::MESSAGE>("\t  --binary : Read the file in binary SuperIO format.");

    log<System::MESSAGE>("\t  --resolution X Y : Set the resolution (AKA window size). Default is 800x800");
  
    log<System::MESSAGE>("\x1b[1m Controls:\x1b[0m");
    log<System::MESSAGE>("  Movement:");
    log<System::MESSAGE>("\t Move with WASD, E and Q to tilt and Shift/Ctrl to go up/Down");
    log<System::MESSAGE>("\t Use +/- to increase/decrease the speed");
    log<System::MESSAGE>("\t Look around holding ALT and moving the mouse");
    log<System::MESSAGE>("\t Rotate the world in XYZ using 123/456");
    log<System::MESSAGE>("  Frame control:");
    log<System::MESSAGE>("\t Press Space to go to the next frame, R to the previous");
    log<System::MESSAGE>("\t Press T to go to the last frame, B takes you to the first one");
    log<System::MESSAGE>("\t Press M to play the frames at 60 FPS, M again to pause");
    log<System::MESSAGE>("\t Press C to take a screenshot in png\t Press L to play and record to a mp4 until L is pressed again");
    log<System::MESSAGE>("  Others:");
    log<System::MESSAGE>("\t Press h to print this help page");  
  
  }

  void System::printWelcome(){
    log<System::MESSAGE>("Welcome to Superpunto. ver. %d.%d! Press h for help", SUPERPUNTO_MAJOR, SUPERPUNTO_MINOR);  
  }

}
