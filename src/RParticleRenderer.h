#ifndef RPARTICLERENDERER_H
#define RPARTICLERENDERER_H
#include "RRenderer.h"
#include "shaders.h"

#include <map>
namespace superpunto {
class RParticleRenderer : public RRenderer {
public:
  RParticleRenderer(std::shared_ptr<System> sys, std::shared_ptr<RWindow> w,
                    std::shared_ptr<Camera> cam, float gscale);
  ~RParticleRenderer();

  void upload_instances(ParticleData pdata) override;

  void update() override;
  void SSAOrad(float inc);

  void draw() override;

  int pick(int x, int y, int pickindex) override;

  void handle_resize(uint fw, uint fh);

  Uint8 *getPixels() override;
  glm::int2 getSize() override;

private:
  bool init_buffers();
  bool init_sphere();
  bool init_instance_vbos();
  bool init_vao();

  bool init_shaders();

  bool init_uniforms();

  void geometry_pass();
  void light_pass();
  void SSAO_pass();

  void render_picked();

  FBO fbo, ssaofbo;
  GBuffer gBuffer;

  VBO sphere_vbos[2];    // Vertex, index
  VBO instances_vbos[3]; // pos, color, radius

  VBO lines_vbo; // lines start/end
  int maxN;
  VAO spheres_vao, dummy_vao, line_vao;
  std::map<std::string, uint> attribs;
  RShaderProgram pr, lightpr, ssaopr, linepr;
  GLuint uniMVP, unimodel, uninormalmodel;
  bool picking = false;
};

void fill_sphere_vbos(VBO &posVBO, VBO &indicesVBO);

} // namespace superpunto
#endif
