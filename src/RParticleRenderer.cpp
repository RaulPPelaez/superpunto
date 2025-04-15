#include "RParticleRenderer.h"
#include "icosphere.h"
#include <tuple>
namespace superpunto {

auto create_sphere_vbos(int lod) {
  VBO posVBO, idxVBO;
  int number_vertex = 0;
  DataLayout dl;
  dl.init(3, 3 * sizeof(float), GL_FLOAT, 0);
  posVBO.init(GL_ARRAY_BUFFER, GL_MAP_READ_BIT, dl);
  idxVBO.init(GL_ELEMENT_ARRAY_BUFFER, GL_MAP_READ_BIT);
  auto [vertices, indices] = generate_icosphere(lod);
  posVBO.initmem(3 * vertices.size() * sizeof(GLfloat),
                 reinterpret_cast<GLfloat *>(vertices.data()));
  idxVBO.initmem(3 * indices.size() * sizeof(GLuint),
                 reinterpret_cast<GLuint *>(indices.data()));
  number_vertex = indices.size() * 3;
  return std::make_tuple(number_vertex, std::move(posVBO), std::move(idxVBO));
}

Sphere::Sphere(int lod) {
  std::tie(number_vertex, vertex_vbos[0], vertex_vbos[1]) =
      create_sphere_vbos(lod);
  System::log<System::DEBUG>("[Sphere] Sphere with %d vertices and %d indices",
                             number_vertex, number_vertex * 3);
}
static constexpr int NLODS = 6;
RParticleRenderer::RParticleRenderer(std::shared_ptr<System> sys,
                                     std::shared_ptr<RWindow> w,
                                     std::shared_ptr<Camera> cam, float gscale)
    : RRenderer(sys, w, cam, gscale), maxN(1), fbo(sys, w->getResolution()),
      ssaofbo(sys, w->getResolution()), gBuffer(sys, w->getResolution()),
      lod_program(NLODS, 4, 800, gscale) {
  sys->log<System::DEBUG>("[ParticleRenderer]Initializing OpenGL...");
  init_buffers();
  init_shaders();
  init_uniforms();
  auto resolution = w->getResolution();
  this->handle_resize(resolution.x, resolution.y);
}

RParticleRenderer::~RParticleRenderer() {}

void RParticleRenderer::handle_resize(uint fw, uint fh) {
  RRenderer::handle_resize(fw, fh);
  fbo.handle_resize(fw, fh);
  ssaofbo.handle_resize(fw, fh);
  gBuffer.handle_resize(fw, fh);
  ssaopr.setUniform<float>("FWIDTH", fw);
  ssaopr.setUniform<float>("FHEIGHT", fh);
  CheckGLError("Error at resize");
}
void RParticleRenderer::init_buffers() {
  sys->log<System::DEBUG>("[ParticleRenderer]Init buffers...     ");
  init_sphere();
  init_instance_vbos();
  init_vao();
}
void RParticleRenderer::init_sphere() {
  for (int lod = 0; lod < NLODS; ++lod) {
    sphere_geometries.emplace_back(Sphere(lod));
  }
  CheckGLError("Error in init_sphere");
}

void RParticleRenderer::init_vao() {
  System::log<System::DEBUG>("[ParticleRenderer]Init VAOs...     ");
  sphere_vaos.resize(sphere_geometries.size());
  for (int i = 0; i < sphere_geometries.size(); ++i) {
    sphere_vaos[i].set_attrib(0, sphere_geometries[i].vertex_vbos[0]);
  }
  CheckGLError("Error in init_vao");
}

void RParticleRenderer::init_instance_vbos() {
  System::log<System::DEBUG>("[ParticleRenderer]Init instance vbos...     ");
  DataLayout dl;
  dl.init(4, 4 * sizeof(float), GL_FLOAT, 0, 1);
  instances_vbos[0].init(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_STORAGE_BIT, dl);
  instances_vbos[1].init(GL_ARRAY_BUFFER, GL_DYNAMIC_STORAGE_BIT, dl);
  dl.init(1, sizeof(float), GL_FLOAT, 0, 1);
  instances_vbos[0].initmem(maxN * sizeof(float) * 4, NULL); // pos
  instances_vbos[1].initmem(maxN * sizeof(float) * 4, NULL); // colors
  CheckGLError("Error in init_instance_vbos");
}

void RParticleRenderer::init_shaders() {
  sys->log<System::DEBUG>("[ParticleRenderer] Init shaders...     ");
  RShader shs[2];
  shs[0].charload(shaders_geom_vs, GL_VERTEX_SHADER);
  shs[1].charload(shaders_geom_fs, GL_FRAGMENT_SHADER);
  pr.init(shs, 2);

  shs[0].charload(shaders_quad_vs, GL_VERTEX_SHADER);
  shs[1].charload(shaders_light_fs, GL_FRAGMENT_SHADER);
  lightpr.init(shs, 2);

  shs[0].charload(shaders_quad_vs, GL_VERTEX_SHADER);
  shs[1].charload(shaders_ssao_fs, GL_FRAGMENT_SHADER);

  ssaopr.init(shs, 2);

  shs[0].charload(shaders_line_vs, GL_VERTEX_SHADER);
  shs[1].charload(shaders_line_fs, GL_FRAGMENT_SHADER);
  linepr.init(shs, 2);

  ssaofbo.setFormat(GL_R32F, GL_RED, GL_FLOAT);
  CheckGLError("Error in init_shaders");
}

void RParticleRenderer::init_uniforms() {
  sys->log<System::DEBUG>("[ParticleRenderer] Init uniforms...    ");
  pr.setUniform<GLint>("picking", 0);
  pr.setUniform<GLint>("drawing_picked", 0);
  pr.setUniform<float>("pickscale", 1.0f);
  pr.setUniform<float>("gscale", this->gscale);
  pr.setUniform<float>("znear", znear);
  pr.setUniform<float>("zfar", zfar);
  lightpr.setUniform<GLint>("ctex", gBuffer.getColorUnit());
  lightpr.setUniform<GLint>("ndtex", gBuffer.getNormalDepthUnit());
  lightpr.setUniform<GLint>("ptex", gBuffer.getPositionUnit());
  lightpr.setUniform<GLint>("SSAOtex", ssaofbo.getTexUnit());
  ssaopr.setUniform<GLint>("ndtex", gBuffer.getNormalDepthUnit());
  ssaopr.setUniform<GLint>("noisetex", gBuffer.getNoiseUnit());
  auto op = sys->getInputOptions();
  ssaopr.setUniform<float>("FWIDTH", (float)op.target_FW);
  ssaopr.setUniform<float>("FHEIGHT", (float)op.target_FH);
  linepr.setUniform<glm::vec3>("color", glm::vec3(1, 1, 1));
  CheckGLError("Error in init_uniforms");
}

void RParticleRenderer::upload_instances(ParticleData pdata) {
  int N = pdata.N;
  if (N > maxN) {
    sys->log<System::DEBUG>(
        "[RParticleRenderer] Resizing instances to a max of %d", N);
    maxN = N;
    instances_vbos[0].reset();
    instances_vbos[1].reset();
    init_instance_vbos();
    init_vao();
  }
  static std::vector<glm::vec4> buffer(N);
  buffer.resize(N);
  // Copy positions+scale
  fori(0, N) {
    buffer[i].x = pdata.pos[3 * i];
    buffer[i].y = pdata.pos[3 * i + 1];
    buffer[i].z = pdata.pos[3 * i + 2];
    buffer[i].w = pdata.scales[i];
  }
  instances_vbos[0].upload(0, 4 * N * sizeof(float), buffer.data());
  // Copy colors
  fori(0, N) {
    buffer[i].x = pdata.colors[3 * i];
    buffer[i].y = pdata.colors[3 * i + 1];
    buffer[i].z = pdata.colors[3 * i + 2];
    buffer[i].w = 1.0f;
  }
  instances_vbos[1].upload(0, 4 * N * sizeof(float), buffer.data());
  this->particles = pdata;
  float3 L = pdata.L;
  box.setSize(glm::vec3(L.x, L.y, L.z));
}

void RParticleRenderer::update() { RRenderer::update(); }

int RParticleRenderer::pick(int x, int y, int pickindex) {
  pr.use();
  pr.setUniform<GLint>("picking", 1);
  this->picking = true;
  geometry_pass();
  pr.use();
  pr.setUniform<GLint>("picking", 0);
  auto pixel = gBuffer.getPixel(x, y);
  uint id = pixel.r + 256u * pixel.g + 256u * 256u * pixel.b +
            256u * 256u * 256u * pixel.a;
  picked[pickindex] = int(id) - 1;
  this->picking = false;
  System::log<System::DEBUG>("Picked %d", pickindex);
  return picked[pickindex];
}

void RParticleRenderer::render_picked() {
  if (picked[0] < 0 && picked[1] < 0)
    return;
  pr.use();
  glLineWidth(5.3f);
  pr.setUniform<GLint>("drawing_picked", 1);
  pr.setUniform<GLfloat>("pickscale", 1.3f);
  int pickedlod = 1;
  auto nvertex = sphere_geometries[pickedlod].number_vertex;
  if (picked[0] >= 0) {
    pr.setUniform<GLint>("pick_id", picked[0]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1,
                     instances_vbos[0].id()); // vec4 pos
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2,
                     instances_vbos[1].id()); // vec4 color
    sphere_vaos[pickedlod].use();
    sphere_geometries[pickedlod].vertex_vbos[0].use(); // vertices
    sphere_geometries[pickedlod].vertex_vbos[1].use(); // indices
    glDrawElementsInstancedBaseInstance(GL_LINE_STRIP, nvertex, GL_UNSIGNED_INT,
                                        NULL, 1, picked[0]);
  }
  if (picked[1] >= 0) {
    pr.setUniform<GLint>("pick_id", picked[1]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1,
                     instances_vbos[0].id()); // vec4 pos
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2,
                     instances_vbos[1].id()); // vec4 color
    sphere_vaos[pickedlod].use();
    sphere_geometries[pickedlod].vertex_vbos[0].use(); // vertices
    sphere_geometries[pickedlod].vertex_vbos[1].use(); // indices
    glDrawElementsInstancedBaseInstance(GL_LINE_STRIP, nvertex, GL_UNSIGNED_INT,
                                        NULL, 1, picked[1]);
  }
  if (picked[1] >= 0 && picked[0] >= 0) {
    glm::vec3 pointA, pointB;
    for (int i = 0; i < 3; ++i) {
      pointA[i] = particles.pos[3 * picked[0] + i] * gscale;
      pointB[i] = particles.pos[3 * picked[1] + i] * gscale;
    }
    linepr.use();
    dummy_vao.use();
    linepr.setUniform<glm::mat4>("MVP", MVP);
    linepr.setUniform<glm::vec3>("pointA", pointA);
    linepr.setUniform<glm::vec3>("pointB", pointB);
    glLineWidth(5.3f);
    glDrawArrays(GL_LINES, 0, 2);
  }
  pr.setUniform<GLfloat>("pickscale", 1.0f);
  pr.setUniform<GLint>("drawing_picked", 0);
  pr.unbind();
}

void RParticleRenderer::geometry_pass() {
  auto resolution = w->getResolution();
  lod_program.sort(particles.N, cam->pos, cam->lookAt(), model, proj,
                   resolution, instances_vbos[0]);
  auto cfg = sys->getInputOptions();
  gBuffer.use();
  glEnable(GL_DEPTH_TEST);
  glClearColor(cfg.bcolor[0], cfg.bcolor[1], cfg.bcolor[2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  pr.use();
  pr.setUniform<glm::mat4>("MVP", MVP);
  glm::mat4 normal_model = transpose(inverse(model));
  pr.setUniform<glm::mat4>("normal_model", normal_model);
  const auto &lod_counts = lod_program.get_counts();
  const GLuint lod_stride = lod_program.get_lod_stride();
  const VBO &index_buffer = lod_program.get_index_buffer();
  // vec4 pos
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, instances_vbos[0].id());
  // vec4 color
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, instances_vbos[1].id());
  // uint indices
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, index_buffer.id());
  System::log<System::DEBUG6>("[RParticleRenderer] Camera position: %f %f %f",
                              cam->pos.x, cam->pos.y, cam->pos.z);
  for (int lod = 0; lod < sphere_geometries.size(); ++lod) {
    GLuint count = lod_counts[lod];
    if (count == 0)
      continue;
    GLuint base = lod * lod_stride;
    sphere_vaos[lod].use();
    sphere_geometries[lod].vertex_vbos[0].use(); // vertices
    sphere_geometries[lod].vertex_vbos[1].use(); // indices
    pr.setUniform<GLint>("lod_base", base);
    System::log<System::DEBUG6>(
        "[RParticleRenderer] Drawing %d instances of LOD %d", count, lod);
    glDrawElementsInstancedBaseInstance(GL_TRIANGLES,
                                        sphere_geometries[lod].number_vertex,
                                        GL_UNSIGNED_INT, NULL, count, base);
  }
  if (!picking) {
    render_picked();
  }
  if (!cfg.nobox){
    box.draw();
  }
}

void RParticleRenderer::light_pass() {
  fbo.use();
  glDisable(GL_DEPTH_TEST);
  lightpr.use();
  lightpr.setUniform<glm::vec3>("viewPos", cam->pos);
  dummy_vao.use();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f) { return a + f * (b - a); }
void RParticleRenderer::SSAO_pass() {
  static const int nsamples = 129;
  static glm::vec4 points[nsamples];
  static bool gen_points = true;
  if (gen_points) {
    fori(0, nsamples) {
      // We will sample depths in a semisphere
      auto randEsp = []() { return rand() / (float)RAND_MAX; };
      glm::vec4 sample = glm::normalize(glm::vec4(
          randEsp() * 2.0 - 1.0, randEsp() * 2.0 - 1.0, randEsp(), 0));
      float scale = float(i) / nsamples;
      // We want the samples to be preferently close
      scale = lerp(0.1f, 1.0f, scale * scale);
      sample *= scale;
      points[i] = sample;
    }
    ssaopr.setUniform<glm::vec4>("points", points, nsamples);
    gen_points = false;
  }
  ssaofbo.use();
  glDisable(GL_DEPTH_TEST);
  ssaopr.use();
  dummy_vao.use();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void RParticleRenderer::draw() {
  geometry_pass();
  SSAO_pass();
  light_pass();
  glDisable(GL_DEPTH_TEST);
  fbo.draw();
  RRenderer::display();
}

Uint8 *RParticleRenderer::getPixels() { return fbo.getColorData(); }

glm::int2 RParticleRenderer::getSize() { return fbo.getSize(); }
} // namespace superpunto
