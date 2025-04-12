#ifndef RFILE_H
#define RFILE_H

#include"defines.h"
#include"System.h"
#include<memory>
#include<vector>
#include<string>
#include<fstream>
#include<stdio.h>
#include<sstream>
#include<iostream>
#include"math_helper.h"
#include"superIO.h"
#include<thread>
#include<future>
#include<atomic>
namespace superpunto{

  typedef unsigned int uint;
  
  struct RColor{
    float r,g,b;
  };


  class ColorParser{
  public:
    ColorParser(ColorParserType tp, uint pid=923302100);

    RColor getColor(uint id);
   
  private:
    RColor id2BGR(uint id);
    ColorParserType tp;
    std::vector<uint> palette;
    uint palette_id;
  };



  struct ParticleData{
    const float *pos=nullptr, *scales=nullptr, *colors=nullptr;
    const float *vel=nullptr; //for arrows
    uint N=0;
    float3 L;
    std::string msg;
    int frame = -1;
  };

  struct RFile{
  private:
    std::shared_ptr<System> sys;

    superIO::superFile in;
    uint maxN;
    float maxScale;
    
    uint current_frame;

    std::atomic<uint> storedFrames;
  
    std::vector<int> Natframe;
  
    std::vector<std::vector<float>> pos, vel, scales, colors;
    std::vector<float3> Lbox;
    std::vector<float3> max_dist;
    std::vector<std::string> msgs;        

    ParticleData current;

    void readAllFrames();
    void readAllFramesAsync(std::future<void> futureObject){
      while(futureObject.wait_for(std::chrono::nanoseconds(1)) == std::future_status::timeout
	    and readNextFrame()){}
    }
  
    bool readNextFrame();
  
    bool iscomment(std::string line);
    void parse_comment(std::string line, std::string &msg, float *L);
    void parseRowNumbers(float *row_numbers, int numberColumns);

    std::atomic<bool> end_of_file;
    bool asyncRead = true;

    std::thread readThread;
    std::promise<void> exitSignal;
    
  public:
    RFile(std::shared_ptr<System> sys):
      sys(sys),
      in(sys->getInputOptions().readFile){
      if(!in.good()){
	sys->log<System::CRITICAL>("[File] Could not open file %s!", sys->getInputOptions().readFile.c_str());
      }
      storedFrames.store(0);
      end_of_file.store(false);
      readNextFrame();
      if(asyncRead){
	auto future = exitSignal.get_future();
	readThread = std::thread(&RFile::readAllFramesAsync, this, std::move(future));	
      }
      else{
	sys->log<System::CRITICAL>("[File] Could not open file %s!", sys->getInputOptions().readFile.c_str());
      }
      setFrame(0);
    }
    ~RFile(){
      if(asyncRead){
	exitSignal.set_value();
	readThread.join();
      }
	
    }
    void setFrame(int frame);
    void setNextFrame();
    void setPreviousFrame();


    ParticleData getCurrentFrameData(){
      return current;
    }
    ParticleData getFrameData(uint frame){
      uint old_frame = current_frame;
      setFrame(frame);
      auto current = getCurrentFrameData();
      setFrame(old_frame);
      return current;
    }

    float3 getMaxDist(){ return max_dist[current_frame];}
    float getMaxScale(){ return maxScale;}
  };

  std::string read_file(const char *fileName);

}
#endif
