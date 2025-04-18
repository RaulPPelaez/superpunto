#include "RFile.h"
#include "math_helper.h"
#include "palettes.h"
#include "superIO.h"
#include <fstream>
#include <sstream>

namespace superpunto {

ColorParser::ColorParser(ColorParserType tp, std::string palette)
    : palette(palette), tp(tp) {
  switch (tp) {
  case ColorParserType::PALETTE:
    if (palette == "superpunto") {
      srand(palette_id);
      palette_hex.resize(1000, 0);
      for (auto &c : palette_hex)
        c = rand() % 0xFFFFFF;
      palette_hex[0] = 0xC0C0C0;
      palette_hex[1] = 0x00FF00;
      palette_hex[2] = 0x00FF00;
      palette_hex[3] = 0xFF0000;
    } else {
      const auto palette_hex_el = superpunto::palettes::colormaps.find(palette);
      if (palette_hex_el != superpunto::palettes::colormaps.end()) {
        for (auto c : palette_hex_el->second) {
          palette_hex.push_back(c);
        }
      } else {
        System::log<System::CRITICAL>("[File] Color palette %s not found",
                                      palette.c_str());
        throw std::invalid_argument(
            "[File] Color palette not found, using default");
      }
    }
    break;
  default:
    break;
  }
}

RColor ColorParser::getColor(float c) {
  int id;
  switch (tp) {
  case ColorParserType::PALETTE:
    if(palette == "superpunto") {
      id = uint(c);
    }
    else{
      id = fabs(c) * palette_hex.size();
    }
    return id2BGR(palette_hex[id % palette_hex.size()]);
    break;
  case ColorParserType::HEXBGR:
    return id2BGR(uint(c));
    break;
  }
  return id2BGR(0xC0C0C0); // Default color
}

RColor ColorParser::id2BGR(uint id) {
  RColor color;
  color.r = ((id >> 16) & 0xFF) / 255.0f;
  color.g = ((id >> 8) & 0xFF) / 255.0f;
  color.b = (id & 0xFF) / 255.0f;
  return color;
}

void RFile::setFrame(int frame) {

  if (frame < 0) {
    // Go to last frame
    if (!asyncRead)
      while (readNextFrame()) {
      }
    else
      while (!end_of_file) {
      }

    frame = storedFrames - 1;
  }
  if (frame >= storedFrames) {
    if (!asyncRead) {
      for (int i = storedFrames; i < frame + 1; i++)
        if (!readNextFrame())
          break;
    } else {
      if (!end_of_file)
        sys->log<System::DEBUG1>("[File] Busy Waiting for frame %d, current %d",
                                 frame, (int)storedFrames);
      while (frame >= storedFrames and !end_of_file) {
      }
    }
  }

  if (frame >= storedFrames) {
    sys->log<System::DEBUG>(
        "[File] Could not read frame %d, only %d frames in file %s", frame,
        (int)storedFrames, sys->getInputOptions().readFile.c_str());
  }

  frame = std::min(frame, int(storedFrames - 1));
  current_frame = frame;
  current.pos = pos[frame].data();
  if (vel.size())
    current.vel = vel[frame].data();
  current.scales = scales[frame].data();
  current.colors = colors[frame].data();
  current.N = Natframe[frame];
  current.msg = msgs[frame];
  if (Lbox[frame].y == 0 && Lbox[frame].x == 0 && Lbox[frame].z == 0)
    current.L = max_dist[frame];
  else
    current.L = Lbox[frame];
  current.frame = frame;

  sys->log<System::DEBUG1>("[File] Frame %d set", frame);
}

void RFile::setNextFrame() { setFrame(current_frame + 1); }
void RFile::setPreviousFrame() { setFrame(current_frame - 1); }

bool RFile::iscomment(std::string line) {
  char iscomment = line[line.find_first_not_of(" \t\r\n")];
  return iscomment == '#';
}

void RFile::readAllFrames() {
  if (!asyncRead)
    while (readNextFrame()) {
    }
  else
    while (!end_of_file) {
    }
}
bool RFile::readNextFrame() {
  if (end_of_file)
    return false;
  sys->log<System::DEBUG2>("[File] Reading frame %d", (int)storedFrames);

  char *line;
  std::string comment_message;
  float3 current_lbox = float3();
  // Read comments

  int numberChars = in.peekLine(line);
  while (in.iscomment(line, numberChars, '#')) {
    parse_comment(std::string(line), comment_message, (float *)&(current_lbox));
    numberChars = in.getNextLine(line);
    numberChars = in.peekLine(line);
    sys->log<System::DEBUG4>("[File] Last read line: %s", line);
  }
  if (numberChars <= 0 or !in.good()) {
    sys->log<System::DEBUG>("[File] End of file reached, total frames read: %d",
                            (int)storedFrames);
    end_of_file = true;
    return false;
  }
  msgs.push_back(comment_message);
  Lbox.push_back(current_lbox);

  const auto readColorMode = sys->getInputOptions().read_color_mode;
  const auto palette = sys->getInputOptions().palette;
  ColorParser cparser(readColorMode, palette);

  std::vector<float> current_pos, current_vel, current_scales, current_colors;
  if (storedFrames > 0) {
    int previousNumberParticles = Natframe[storedFrames - 1];
    current_pos.reserve(3 * previousNumberParticles);
    current_vel.reserve(3 * previousNumberParticles);
    current_scales.reserve(previousNumberParticles);
    current_colors.reserve(3 * previousNumberParticles);
  }

  uint numberParticles = 0;
  int numberColumns = 0;
  float tmp[8];
  float current_maxScale = 0.0f;
  float3 current_maxDist = {0, 0, 0};
  // Read up to the next comment or file end
  while (!in.eof() and in.good()) {
    numberChars = in.getNextLine(line);
    if (numberChars <= 0 or in.iscomment(line, numberChars, '#'))
      break;
    // Count number of columns from first row
    if (numberParticles == 0) {
      std::stringstream is(line);
      float number;
      while (is >> number)
        numberColumns++;
      if (numberColumns == 0)
        break;
      // I do not understand more than 8 columns
      numberColumns = std::min(numberColumns, 8);
    }
    // Try to transform the current line to numbers
    if (superIO::string2numbers(line, numberChars, numberColumns, tmp) < 0) {
      break;
    }
    // If successful store particle data
    numberParticles++;

    parseRowNumbers(tmp, numberColumns);
    fori(0, 3) current_pos.emplace_back(tmp[i]);
    current_maxDist.x = std::max(current_maxDist.x, std::abs(tmp[0]));
    current_maxDist.y = std::max(current_maxDist.y, std::abs(tmp[1]));
    current_maxDist.z = std::max(current_maxDist.z, std::abs(tmp[2]));
    current_maxScale = std::max(current_maxScale, tmp[3]);
    current_scales.emplace_back(tmp[3]);
    auto acolor = cparser.getColor(tmp[4]);
    current_colors.emplace_back(acolor.r);
    current_colors.emplace_back(acolor.g);
    current_colors.emplace_back(acolor.b);
    if (numberColumns > 5) {
      fori(5, 8) current_vel.emplace_back(tmp[i]);
    }
  }
  if (in.eof() or !in.good() or numberChars <= 0) {
    sys->log<System::DEBUG>("[File] End of file reached, total frames read: %d",
                            storedFrames + 1);
    end_of_file = true;
  }

  if (numberParticles < 1) {
    sys->log<System::DEBUG>("[File] Empty frame detected: %d",
                            (int)storedFrames);
    if (!in.good() or numberChars <= 0) {
      sys->log<System::DEBUG>(
          "[File] End of file reached, total frames read: %d",
          (int)storedFrames);
      end_of_file = true;
      return false;
    }
    return false;
  }
  sys->log<System::DEBUG1>(
      "[File] Frame %d read with %d particles, %d columns per particle",
      (int)storedFrames, numberParticles, numberColumns);
  Natframe.push_back(numberParticles);
  current_pos.shrink_to_fit();
  pos.emplace_back(std::move(current_pos));
  if (current_vel.size()) {
    current_vel.shrink_to_fit();
    vel.emplace_back(std::move(current_vel));
  }
  current_scales.shrink_to_fit();
  scales.emplace_back(std::move(current_scales));
  current_colors.shrink_to_fit();
  colors.emplace_back(std::move(current_colors));
  storedFrames++;
  maxN = std::max(maxN, numberParticles);
  maxScale = std::max(maxScale, current_maxScale);
  max_dist.push_back(current_maxDist);
  return true;
}

// Takes the number of columns read in a row and reorganizes them such that for
// a given particle:
//
//   row[0,1,2] = pos
//   row[3] = radius;
//   row[4] = color;
//   row[5,6,7] = vel; //For arrow mode
void RFile::parseRowNumbers(float *row_numbers, int numberColumns) {
  if (numberColumns < 4) {
    if (numberColumns == 1)
      row_numbers[1] = 0.0;
    if (numberColumns == 2)
      row_numbers[2] = 0.0;
    row_numbers[3] = 0.5;
    row_numbers[4] = 0;
  }
  if (numberColumns == 4) {
    std::swap(row_numbers[3], row_numbers[4]);
    row_numbers[3] = 1.0f;
  }
  if (numberColumns == 6) {
    // X Y Z R C Vx Vy Vz
    std::swap(row_numbers[5], row_numbers[7]); // Vx->Vz
    std::swap(row_numbers[4], row_numbers[6]); // C->Vy
    std::swap(row_numbers[3], row_numbers[5]); // R->Vx

    row_numbers[4] = 0.0f; // Default color
  }
  if (numberColumns == 7) {
    std::swap(row_numbers[6], row_numbers[7]); // Vy->Vz
    std::swap(row_numbers[5], row_numbers[6]); // Vx->Vy
    std::swap(row_numbers[4], row_numbers[5]); // C->Vx
    std::swap(row_numbers[3], row_numbers[4]); // R->C
    row_numbers[3] = 1.0f;                     // Default radius
  }
}

float get_flag(std::string flag, std::string &line) {
  float res = 0.0f;
  size_t pos = 0;
  std::string token;
  pos = line.find(flag);
  if (pos != std::string::npos) {
    line.erase(0, pos + flag.length());
    pos = line.find(std::string(";"));
    token = line.substr(0, pos);
    res = std::stof(token, NULL);
    line.erase(0, pos + 1);
  }
  return res;
}
void RFile::parse_comment(std::string line, std::string &msg, float *L) {
  static std::vector<std::string> delims;
  static std::string delim2 = ";";
  static bool init = false;
  if (!init) {
    delims.push_back("Lx=");
    delims.push_back("Ly=");
    delims.push_back("Lz=");
    init = true;
  }

  fori(0, 3) { L[i] = get_flag(delims[i], line); }
  float Lt = get_flag(std::string("L="), line);
  if (Lt != 0.0f)
    L[0] = L[1] = L[2] = Lt;

  msg = line;
}

std::string read_file(const char *fileName) {
  std::ifstream in(fileName);
  std::string contents((std::istreambuf_iterator<char>(in)),
                       std::istreambuf_iterator<char>());
  return contents;
}

} // namespace superpunto
