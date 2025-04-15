#ifndef SYSTEM_H
#define SYSTEM_H
#include<string>
#include<cstring>
#include"defines.h"
#include<cstdio>
#include<GL/glew.h>
#include<SDL2/SDL.h>
#define SUPERPUNTO_MAJOR 5
#define SUPERPUNTO_MINOR 0
#define xSPUNTOSTR(s) SPUNTOSTR(s)
#define SPUNTOSTR(s) #s
#ifndef USEFONT
#define USEFONT UbuntuMono-Regular.ttf
#endif

namespace superpunto{
  enum class ColorParserType{PALETTE, RANGE, HEXBGR, DEFAULT};
  enum class RenderType{PARTICLES, ARROWS, NONE};

  struct Options{
    bool record_movie = false;
    int frames_between_screenshots = 2;
    float bcolor[3] = {0,0,0};
    std::string fontName = xSPUNTOSTR(USEFONT);
    std::string readFile = std::string("/dev/stdin");
    RenderType render_type = RenderType::PARTICLES;
    ColorParserType read_color_mode = ColorParserType::PALETTE;
    bool binary_read_mode = false;
    bool noaxis = false, nobox = false;
    int target_FW = 800, target_FH = 600; //input Resolution
    uint palette_id = 923302100;
    float fov = 0.7854; //45º
  };

#ifdef MAXLOGLEVEL
  constexpr int maxLogLevel = MAXLOGLEVEL;
#else
  constexpr int maxLogLevel = 6;
#endif


  class System{
    Options options;
    int m_argc = 0;
    char ** m_argv = nullptr;

    void parseArguments();
    void printWelcome();
  public:
    System(): System(0, nullptr){}
    System(int argc, char *argv[]): m_argc(argc), m_argv(argv){
      printWelcome();
      parseArguments();

    }

    Options getInputOptions(){ return options; }

    enum level{CRITICAL=0, ERROR, WARNING, MESSAGE, STDERR, STDOUT,
	       DEBUG, DEBUG1, DEBUG2, DEBUG3, DEBUG4, DEBUG5, DEBUG6, DEBUG7};
    template<int level>
    static inline void log(char const *fmt, ...) {
      if(level<=maxLogLevel){
	if(level==CRITICAL) {
	  va_list args;
	  va_start(args, fmt);
	  fprintf(stderr, "\e[101m[CRITICAL] ");
	  vfprintf(stderr, fmt, args);
	  fprintf(stderr, "\e[0m\n");
	  exit(1);
	}
	if(level==ERROR){
	  va_list args;
	  va_start(args, fmt);
	  fprintf(stderr, "\e[91m[ ERROR ] \e[0m");
	  vfprintf(stderr, fmt, args);
	  fprintf(stderr, "\n");
	}
	if(level==WARNING){
	  va_list args;
	  va_start(args, fmt);
	  fprintf(stderr, "\e[93m[WARNING] \e[0m");
	  vfprintf(stderr, fmt, args);
	  fprintf(stderr, "\n");
	}
	if(level==MESSAGE){
	  va_list args;
	  va_start(args, fmt);
	  fprintf(stderr, "\e[92m[MESSAGE] \e[0m");
	  vfprintf(stderr, fmt, args);
	  fprintf(stderr, "\n");
	}
	if(level==STDERR){
	  va_list args;
	  va_start(args, fmt);
	  vfprintf(stderr, fmt, args);
	  fprintf(stderr, "\n");
	}

	if(level==STDOUT){
	  va_list args;
	  va_start(args, fmt);
	  vfprintf(stdout, fmt, args);
	  fprintf(stdout, "\n");
	}
	if(level==DEBUG){
	  va_list args;
	  va_start(args, fmt);
	  fprintf(stderr, "\e[96m[ DEBUG ] \e[0m");
	  vfprintf(stderr, fmt, args);
	  fprintf(stderr, "\n");
	}
	if(level>=DEBUG1 && level<=DEBUG7){
	  va_list args;
	  va_start(args, fmt);
	  fprintf(stderr, "\e[96m[ DEBUG ] \e[0m");
	  vfprintf(stderr, fmt, args);
	  fprintf(stderr, "\n");
	}
      }
    }

    int getargc(){ return this->m_argc;}
    const char ** getargv(){return (const char **)this->m_argv;}

    void printHelp();
  };




}
#endif
