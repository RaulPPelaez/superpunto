#pragma once
#include "System.h"
#include <memory>
#include <string>
#include <filesystem>
namespace superpunto {

class MovieRecorder {
public:
  MovieRecorder(std::shared_ptr<System> sys, int width, int height,
                const std::string& baseFilename,
                int framerate = 60, int bitrate = 24000);

  bool isOpen() const;
  void writeFrame(const Uint8* pixels);

private:
  std::shared_ptr<System> sys;
  std::filesystem::path outputDir;
  int width, height;
  int framerate;
  int frameCounter = 0;
  std::string baseFilename;
};
} // namespace superpunto
