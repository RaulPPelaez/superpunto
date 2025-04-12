#pragma once
#include "System.h"
#include <memory>
#include <string>
namespace superpunto {

class MovieRecorder {
public:
  MovieRecorder(std::shared_ptr<System> sys, int width, int height,
                const std::string &baseFilename,int framerate = 60, int bitrate = 24000);
  ~MovieRecorder();

  bool isOpen() const;

  void writeFrame(const void *pixels);

  void close();

private:

  FILE *ffmpeg = nullptr;
  int width, height;
};

} // namespace superpunto
