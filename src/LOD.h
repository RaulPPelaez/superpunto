#pragma once

#include "RGL.h"
#include <memory>
#include <vector>

namespace superpunto {

class LODProgram {
public:
  LODProgram(int num_lods, float min_dist, float max_dist, float gscale);
  void sort(int num_particles, const glm::vec3 &camera_pos,
            const glm::mat4 &view, const glm::mat4 &model,
            const glm::mat4 &proj, const glm::int2 resolution,
            const VBO &pos_buffer);
  void resize(int max_particles);
  const VBO &get_index_buffer() const;
  const std::vector<GLuint> &get_counts() const;
  int get_lod_stride() const { return this->max_particles; }

private:
  int num_lods;
  int max_particles;
  float min_dist, max_dist, gscale;
  VBO index_buffer;
  VBO counter_buffer;
  std::vector<GLuint> counts;
  RShaderProgram compute_shader;
};

} // namespace superpunto
