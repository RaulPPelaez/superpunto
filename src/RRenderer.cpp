#include "RRenderer.h"

namespace superpunto {
RRenderer::RRenderer(std::shared_ptr<System> sys, std::shared_ptr<RWindow> in_w,
                     std::shared_ptr<Camera> cam, float gscale)
    : sys(sys), cam(cam), gscale(gscale), box(gscale, &MVP),
      axis(sys, in_w, &MVP, glm::vec3(-10, 0, 0)), textRenderer(sys, in_w),
      w(in_w) {
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

  if (!sys->getInputOptions().noaxis)
    axis.draw(cam->pos);
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

#include "shaders.h"

RAxis::RAxis(std::shared_ptr<System> sys, std::shared_ptr<RWindow> w,
             glm::mat4 *MVP, glm::vec3 origin)
    : MVP(MVP), axislength(5.0f), origin(origin), sys(sys), w(w), Xtext(sys, w),
      Ytext(sys, w), Ztext(sys, w) {
  auto op = sys->getInputOptions();
  Xtext.setFont(op.fontName.c_str(), 45);
  Ytext.setFont(op.fontName.c_str(), 45);
  Ztext.setFont(op.fontName.c_str(), 45);

  Xtext.setText("X", 0, 0);
  Ytext.setText("Y", 0, 0);
  Ztext.setText("Z", 0, 0);

  RShader shs[3];
  shs[0].charload(shaders_axis_vs, GL_VERTEX_SHADER);
  shs[1].charload(shaders_axis_gs, GL_GEOMETRY_SHADER);
  shs[2].charload(shaders_axis_fs, GL_FRAGMENT_SHADER);
  pr.init(shs, 3);
}

void RAxis::draw(glm::vec3 campos) {
  glm::vec4 xpos =
      *MVP * (glm::vec4(origin, 0) + glm::vec4(axislength, 0, 0, 1));
  glm::vec4 ypos =
      *MVP * (glm::vec4(origin, 0) + glm::vec4(0, axislength, 0, 1));
  glm::vec4 zpos =
      *MVP * (glm::vec4(origin, 0) + glm::vec4(0, 0, axislength, 1));

  xpos.x /= xpos.w;
  ypos.x /= ypos.w;
  zpos.x /= zpos.w;
  xpos.y /= xpos.w;
  ypos.y /= ypos.w;
  zpos.y /= zpos.w;

  float dist = glm::length(campos);
  auto resolution = w->getResolution();
  Xtext.setText("X", 0.5f * (xpos.x + 1) * resolution.x,
                resolution.y - 0.5f * (xpos.y + 1) * resolution.y, 35.f / dist);
  Ytext.setText("Y", 0.5f * (ypos.x + 1) * resolution.x,
                resolution.y - 0.5f * (ypos.y + 1) * resolution.y, 35.f / dist);
  Ztext.setText("Z", 0.5f * (zpos.x + 1) * resolution.x,
                resolution.y - 0.5f * (zpos.y + 1) * resolution.y, 35.f / dist);

  glEnable(GL_DEPTH_TEST);
  pr.use();

  glUniformMatrix4fv(glGetUniformLocation(pr.id(), "MVP"), 1, GL_FALSE,
                     glm::value_ptr(*MVP));
  glUniform1f(glGetUniformLocation(pr.id(), "axislength"), 5.0f);
  glUniform3f(glGetUniformLocation(pr.id(), "origin"), origin.x, origin.y,
              origin.z);

  glLineWidth(5.0f);
  dummy_vao.use();
  glDrawArrays(GL_POINTS, 0, 3);
  dummy_vao.unbind();
  pr.unbind();
  glLineWidth(2.0f);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Xtext.draw();
  Ytext.draw();
  Ztext.draw();

  glDisable(GL_BLEND);
}

} // namespace superpunto
