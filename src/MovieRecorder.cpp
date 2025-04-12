#include "MovieRecorder.h"
#include "RPNG.h"
#include "System.h"
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;

namespace superpunto {

MovieRecorder::MovieRecorder(std::shared_ptr<System> sys, int width, int height,
                             const std::string &baseFilename, int framerate,
                             int /*bitrate*/)
    : sys(sys), width(width), height(height), framerate(framerate),
      baseFilename(baseFilename) {

  static int folderCounter = 0;
  std::ostringstream folderName;
  folderName << baseFilename << "_" << folderCounter++;
  outputDir = folderName.str();

  try {
    fs::create_directories(outputDir);
    sys->log<System::MESSAGE>("Recording frames to: %s",
                              outputDir.string().c_str());
    sys->log<System::MESSAGE>("You may use this command to convert to mp4:\n"
                              "ffmpeg -r %d -i %s/frame_%%05d.png -c:v libx264 "
                              "-pix_fmt yuv420p %s.mp4",
                              framerate, outputDir.string().c_str(),
                              outputDir.string().c_str());
  } catch (const std::exception &e) {
    sys->log<System::WARNING>("Failed to create output directory: %s",
                              e.what());
  }
}

bool MovieRecorder::isOpen() const { return fs::exists(outputDir); }

void MovieRecorder::writeFrame(const Uint8 *pixels) {
  if (!isOpen())
    return;

  std::ostringstream filename;
  filename << outputDir.string() << "/frame_" << std::setw(5)
           << std::setfill('0') << frameCounter++ << ".png";

  savePNG(filename.str().c_str(), pixels, width, height);
}

} // namespace superpunto
