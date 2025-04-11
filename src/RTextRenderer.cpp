#include "RTextRenderer.h"
#include "glm/glm.hpp"
#include "resources.h"

namespace superpunto {
std::shared_ptr<RShaderProgram> RTextRenderer::pr;
std::shared_ptr<VAO> RTextRenderer::dummy_vao;
int RTextRenderer::instance_counter = 0;
RTextRenderer::RTextRenderer(std::shared_ptr<System> sys,
                             std::shared_ptr<RWindow> w)
    : sys(sys), size(glm::vec2(0)), pos(glm::vec2(0)), size_factor(1.0f),
      font(nullptr), w(w) {
  instance_counter++;
}

RTextRenderer::~RTextRenderer() {
  if (instance_counter)
    instance_counter--;
  TTF_CloseFont(font);
  font = NULL;

  if (instance_counter == 0) {
    if (TTF_WasInit()) {
      pr.reset();
      dummy_vao.reset();
      TTF_Quit();
    }
  }
}
#include "shaders.h"
bool RTextRenderer::setFont(const char *fontName, int size) {
  if (!TTF_WasInit()) {
    TTF_Init();
    RShader shs[2];
    shs[0].charload(shaders_text_vs, GL_VERTEX_SHADER);
    shs[1].charload(shaders_text_fs, GL_FRAGMENT_SHADER);
    pr = std::make_shared<RShaderProgram>();
    pr->init(shs, 2);
    dummy_vao = std::make_shared<VAO>();
  }
  this->font =
      TTF_OpenFont(("../resources/" + std::string(fontName)).c_str(), size);
  if (!font) {
    sys->log<System::ERROR>("[TextRenderer] Could not load font %s!!",
                            fontName);
    const char *error = TTF_GetError();
    sys->log<System::ERROR>("[TextRenderer] TTF Error: %s", error);
  }

  return true;
}
bool RTextRenderer::setText(const char *text, int x, int y, float sf) {
  pos.x = x;
  pos.y = y;
  this->size_factor = sf;

  SDL_Color color = {255, 255, 255, 0};
  SDL_Surface *raw_surf = TTF_RenderText_Blended(this->font, text, color);
  SDL_Surface *surf =
      SDL_ConvertSurfaceFormat(raw_surf, SDL_PIXELFORMAT_RGBA32, 0);
  SDL_FreeSurface(raw_surf);

  if (tex.id() == 0) {
    tex.init(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, glm::int2(surf->w, surf->h));
    pr->setFlag("ctex", tex.getUnit());
  } else
    tex.resize(surf->w, surf->h);
  this->size = glm::vec2(surf->w / (float)w->getResolution().x,
                         surf->h / (float)w->getResolution().y);
  pr->use();
  glUniform2f(glGetUniformLocation(pr->id(), "size"), size.x, size.y);
  glUniform2f(glGetUniformLocation(pr->id(), "pos"), pos.x, pos.y);
  pr->unbind();

  tex.upload(surf->pixels);
  SDL_FreeSurface(surf);
  return true;
}

void RTextRenderer::handle_resize(uint fw, uint fh) {

  this->size =
      glm::vec2(tex.getSize().x / (float)fw, tex.getSize().y / (float)fh);
  pr->use();
  glUniform2f(glGetUniformLocation(pr->id(), "size"), size.x, size.y);
  pr->unbind();
}
bool RTextRenderer::move(int x, int y) {
  pos.x = x;
  pos.y = y;
  pr->use();
  glUniform2f(glGetUniformLocation(pr->id(), "size"), size.x, size.y);
  glUniform2f(glGetUniformLocation(pr->id(), "pos"), pos.x, pos.y);
  pr->unbind();
  return true;
}
void RTextRenderer::draw() {
  pr->setFlag("ctex", tex.getUnit());
  pr->use();
  glUniform1f(glGetUniformLocation(pr->id(), "fontsize_multiplicator"),
              size_factor);
  glUniform2f(glGetUniformLocation(pr->id(), "size"), size.x, size.y);
  glUniform2f(glGetUniformLocation(pr->id(), "pos"), pos.x, pos.y);
  glUniform1f(glGetUniformLocation(pr->id(), "FWIDTH"),
              (float)w->getResolution().x);
  glUniform1f(glGetUniformLocation(pr->id(), "FHEIGHT"),
              (float)w->getResolution().y);

  dummy_vao->use();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  // dummy_vao.unbind();
  // pr.unbind();
}

} // namespace superpunto
