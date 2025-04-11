#include "RRenderer.h"
#include "shaders.h"

namespace superpunto {
RRenderer::RRenderer(std::shared_ptr<System> sys, std::shared_ptr<RWindow> in_w,
                     std::shared_ptr<Camera> cam, float gscale)
    : sys(sys), cam(cam), gscale(gscale), box(gscale, &MVP), axis(sys, in_w),
      textRenderer(sys, in_w), w(in_w) {
  picked[0] = picked[1] = -1;
  textRenderer.setFont(sys->getInputOptions().fontName.c_str(), 45);
  auto resolution = w->getResolution();
  auto op = sys->getInputOptions();
  proj =
      glm::perspective(op.fov, resolution.x / (float)resolution.y, znear, zfar);
  model = glm::mat4();
  view = cam->lookAt();
  rotate_model(M_PI / 4.0f, 1.0f, 0.0f, 0.0f);

  MVP = proj * view * model;
}

void RRenderer::handle_resize(uint fw, uint fh) {
  glViewport(0, 0, fw, fh);
  auto op = sys->getInputOptions();
  float ar = fw / (float)fh;
  proj = glm::perspective(op.fov, ar, znear, zfar);
  textRenderer.handle_resize(fw, fh);
}

void RRenderer::rotate_model(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
  model = glm::rotate(model, angle, glm::vec3(x, y, z));
}

void RRenderer::update() {
  view = cam->lookAt();
  MVP = proj * view * model;

  cam->update();
}

void RRenderer::handle_event(SDL_Event &e) {}

void RRenderer::display() {

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  textRenderer.draw();
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);

  if (!sys->getInputOptions().noaxis) {
    glm::mat4 view = cam->lookAt();
    glm::ivec2 res = w->getResolution();
    axis.draw(view, model, res);
  }
}

void RRenderer::drawText(const char *text, int x, int y) {
  textRenderer.setText(text, x, y);
}

int RRenderer::pick(int x, int y, int pickindex) { return -1; }

Uint8 *RRenderer::getPixels() {
  static std::vector<Uint8> cdata;
  auto resolution = w->getResolution();
  size_t cdatasize = resolution.x * resolution.y * 4;
  if (cdata.size() != cdatasize)
    cdata.resize(cdatasize);

  glReadPixels(0, 0, resolution.x, resolution.y, GL_RGBA, GL_UNSIGNED_BYTE,
               (void *)cdata.data());
  return cdata.data();
}

glm::int2 RRenderer::getSize() {
  auto resolution = w->getResolution();
  return glm::int2(resolution.x, resolution.y);
}



RAxis::RAxis(std::shared_ptr<System> sys, std::shared_ptr<RWindow> w)
    : sys(sys), w(w), Xtext(sys, w), Ytext(sys, w), Ztext(sys, w) {

  RShader shs[2];
  shs[0].charload(shaders_axis_vs, GL_VERTEX_SHADER);
  shs[1].charload(shaders_axis_fs, GL_FRAGMENT_SHADER);
  pr.init(shs, 2);
  auto op = sys->getInputOptions();
  Xtext.setFont(op.fontName.c_str(), 45);
  Ytext.setFont(op.fontName.c_str(), 45);
  Ztext.setFont(op.fontName.c_str(), 45);

  std::vector<float> axis_lines = {
      // X Axis (Red)
      0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.8f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

      // Y Axis (Green)
      0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.8f, 0.0f, 0.0f, 1.0f, 0.0f,

      // Z Axis (Blue)
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.8f, 0.0f, 0.0f, 1.0f};
  vbo.init(GL_ARRAY_BUFFER, GL_DYNAMIC_STORAGE_BIT);
  vbo.initmem(axis_lines.size() * sizeof(float), axis_lines.data());

  vao.use();
  vbo.use();
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6,
                        (void *)(sizeof(float) * 3));
  vao.unbind();
}

void RAxis::draw(const glm::mat4 &view, const glm::mat4 &model,
                 const glm::ivec2 &resolution) {
  glm::mat4 proj = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
  glm::mat4 modelTransform =
      glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.0f));
  modelTransform = glm::scale(modelTransform, glm::vec3(0.6f));
  glm::mat3 camRot = glm::mat3(view);
  glm::mat3 modelRot = glm::mat3(model);
  glm::mat4 rotation = glm::mat4(camRot * modelRot);

  glm::mat4 MVP = proj * modelTransform * rotation;

  glViewport(10, 10, 100, 100);
  glDisable(GL_DEPTH_TEST);

  glLineWidth(10.0f);

  pr.use();
  glUniformMatrix4fv(glGetUniformLocation(pr.id(), "MVP"), 1, GL_FALSE,
                     glm::value_ptr(MVP));
  vao.use();
  glDrawArrays(GL_LINES, 0, 6);
  vao.unbind();
  pr.unbind();

  // Project axis tips to normalized device coordinates

  glm::vec4 x_tip =
      modelTransform * rotation * glm::vec4(1.8f, 0.0f, 0.0f, 1.0f);
  glm::vec4 y_tip =
      modelTransform * rotation * glm::vec4(0.0f, 1.8f, 0.0f, 1.0f);
  glm::vec4 z_tip =
      modelTransform * rotation * glm::vec4(0.0f, 0.0f, 1.8f, 1.0f);

  x_tip /= x_tip.w;
  y_tip /= y_tip.w;
  z_tip /= z_tip.w;

  float vx = 10.0f, vy = 10.0f, vw = 100.0f, vh = 100.0f;
  glm::vec2 x_screen = glm::vec2(vx + (x_tip.x * 0.5f + 0.2f) * vw,
                                 vy + (x_tip.y * 0.5f + 0.5f) * vh);
  glm::vec2 y_screen = glm::vec2(vx + (y_tip.x * 0.5f + 0.2f) * vw,
                                 vy + (y_tip.y * 0.5f + 0.5f) * vh);
  glm::vec2 z_screen = glm::vec2(vx + (z_tip.x * 0.5f + 0.2f) * vw,
                                 vy + (z_tip.y * 0.5f + 0.5f) * vh);

  glViewport(0, 0, resolution.x, resolution.y);

  Xtext.setText("X", x_screen.x, resolution.y - x_screen.y);
  Ytext.setText("Y", y_screen.x, resolution.y - y_screen.y);
  Ztext.setText("Z", z_screen.x, resolution.y - z_screen.y);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Xtext.draw();
  Ytext.draw();
  Ztext.draw();

  glDisable(GL_BLEND);
}

} // namespace superpunto
