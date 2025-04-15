#ifndef RPARTICLERENDERER_H
#define RPARTICLERENDERER_H
#include "RRenderer.h"
#include "shaders.h"
#include "LOD.h"
#include <map>
namespace superpunto {
struct Sphere {
  Sphere(int lod);

  VBO vertex_vbos[2];    // Vertex, index
  int number_vertex;
};

class RParticleRenderer : public RRenderer {
public:
  RParticleRenderer(std::shared_ptr<System> sys, std::shared_ptr<RWindow> w,
                    std::shared_ptr<Camera> cam, float gscale);
  ~RParticleRenderer();

  void upload_instances(ParticleData pdata) override;

  void update() override;

  void draw() override;

  int pick(int x, int y, int pickindex) override;

  void handle_resize(uint fw, uint fh);

  Uint8 *getPixels() override;
  glm::int2 getSize() override;

private:
  void init_buffers();
  void init_sphere();
  void init_instance_vbos();
  void init_vao();

  void init_shaders();

  void init_uniforms();

  void geometry_pass();
  void light_pass();
  void SSAO_pass();

  void render_picked();

  FBO fbo, ssaofbo;
  GBuffer gBuffer;

  std::vector<Sphere> sphere_geometries;
  std::vector<VAO> sphere_vaos;
  VBO vertex_vbos[2];
  VBO instances_vbos[2]; // pos+scale, color
  int maxN;
  VAO dummy_vao;
  RShaderProgram pr, lightpr, ssaopr, linepr;
  bool picking = false;
  LODProgram lod_program;
};

} // namespace superpunto
#endif
