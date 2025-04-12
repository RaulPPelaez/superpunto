#include "MovieRecorder.h"

namespace superpunto {

MovieRecorder::MovieRecorder(std::shared_ptr<System> sys, int width, int height,
                             const std::string &baseFilename,
                             int framerate, int bitrate)
    : width(width), height(height) {
  static int file_counter = 0;
  std::string filename =
      baseFilename + "_" + std::to_string(file_counter++) + ".mp4";
  std::string cmd = "ffmpeg -loglevel panic -y -f rawvideo -s " +
                    std::to_string(width) + "x" + std::to_string(height) +
                    " -pix_fmt rgba -r " + std::to_string(framerate) +
                    " -i - -vf vflip -an -b:v " + std::to_string(bitrate) +
                    "k " + "\"" + filename + "\"";
  sys->log<System::MESSAGE>("Encoding video with command: %s", cmd.c_str());
  ffmpeg = ::popen(cmd.c_str(), "w");
  if (!ffmpeg) {
    sys->log<System::ERROR>("Failed to open ffmpeg process.");
  }
}

MovieRecorder::~MovieRecorder() { close(); }

bool MovieRecorder::isOpen() const { return ffmpeg != nullptr; }

void MovieRecorder::writeFrame(const void *pixels) {
  if (isOpen()) {
    std::fwrite(pixels, width * height * 4, 1, ffmpeg);
  }
}

void MovieRecorder::close() {
  if (ffmpeg) {
    std::fflush(ffmpeg);
    ::pclose(ffmpeg);
    ffmpeg = nullptr;
  }
}

} // namespace superpunto
