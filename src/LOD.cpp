#include "LOD.h"
#include "glm/gtc/type_ptr.hpp"
#include "shaders.h"

namespace superpunto {

LODProgram::LODProgram(int num_lods, float min_dist, float max_dist,
                       float gscale)
    : num_lods(num_lods), max_particles(0), min_dist(min_dist),
      max_dist(max_dist), counts(num_lods, 0), gscale(gscale) {
  counter_buffer.init(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_STORAGE_BIT);
  counter_buffer.initmem(num_lods * sizeof(GLuint), nullptr);
  RShader sh;
  sh.charload(shaders_lod_cs, GL_COMPUTE_SHADER);
  compute_shader.init(&sh, 1);
}

void LODProgram::sort(int num_particles, const glm::vec3 &cam_pos,
                      const glm::mat4 &view, const glm::mat4 &model,
                      const glm::mat4 &proj, const glm::int2 resolution,
                      const VBO &pos_buffer) {
  if (num_particles > max_particles) {
    this->resize(num_particles);
  }
  compute_shader.use();
  std::vector<GLuint> zero(num_lods, 0);
  counter_buffer.upload(0, num_lods * sizeof(GLuint), zero.data());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, pos_buffer.id());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, index_buffer.id());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, counter_buffer.id());
  compute_shader.setUniform<GLfloat>("lod_pixel_min", min_dist);
  compute_shader.setUniform<GLfloat>("lod_pixel_max", max_dist);
  compute_shader.setUniform<GLfloat>("gscale", gscale);
  compute_shader.setUniform<GLint>("num_lods", num_lods);
  compute_shader.setUniform<glm::ivec2>("screen_resolution", resolution);
  const auto modelview = view * model;
  compute_shader.setUniform<glm::mat4>("modelview", modelview);
  compute_shader.setUniform<glm::mat4>("projection", proj);
  glDispatchCompute((num_particles + 255) / 256, 1, 1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
                  GL_ATOMIC_COUNTER_BARRIER_BIT);
  counter_buffer.use();
  glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, num_lods * sizeof(GLuint),
                     counts.data());
  CheckGLError("Error in LODProgram::sort");
}

void LODProgram::resize(int new_max_particles) {
  if (new_max_particles > max_particles) {
    max_particles = new_max_particles;
    index_buffer.reset();
    index_buffer.init(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_STORAGE_BIT);
    index_buffer.initmem(num_lods * new_max_particles * sizeof(GLuint),
                         nullptr);
    compute_shader.setUniform<GLint>("lod_stride", max_particles);
  }
}

const VBO &LODProgram::get_index_buffer() const { return index_buffer; }

const std::vector<GLuint> &LODProgram::get_counts() const { return counts; }

} // namespace superpunto
