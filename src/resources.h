#pragma once
#include <cmrc/cmrc.hpp>
CMRC_DECLARE(superpunto_resources);
namespace superpunto_resources{
  auto getResourceFile(std::string path){
    auto fs = cmrc::superpunto_resources::get_filesystem();
    if(!fs.exists(path)){
      throw std::runtime_error("Failed to load " + path + " from resources");
    }
    return fs.open(path);
  }
}
