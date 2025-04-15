#pragma once
#include <vector>
#include <tuple>
#include <glm/glm.hpp>
#include <GL/glew.h>

namespace superpunto {

  using Vertex = glm::vec3;
  using Face = glm::uvec3;

std::tuple<std::vector<Vertex>, std::vector<Face>> generate_icosphere(int nu);


} // namespace superpunto
