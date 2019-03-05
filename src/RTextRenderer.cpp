#include"RTextRenderer.h"

bool RTextRenderer::SDL_ttf_init = false;
RShaderProgram* RTextRenderer::pr;
VAO* RTextRenderer::dummy_vao;

RTextRenderer::RTextRenderer():
  font(nullptr),
  size_factor(1.0f),
  size(glm::vec2(0)),
  pos(glm::vec2(0)){};

#include"shaders.h"
bool RTextRenderer::setFont(const char * fontName, int size){
  if(!SDL_ttf_init){
    TTF_Init();
    RShader shs[2];
    shs[0].charload(shaders_text_vs , GL_VERTEX_SHADER);
    shs[1].charload(shaders_text_fs , GL_FRAGMENT_SHADER);
    pr = new RShaderProgram;
    pr->init(shs, 2);
    SDL_ttf_init = true;
    dummy_vao = new VAO;
  }
  this->font = TTF_OpenFont(fontName, size);
  if(!font) cerr<<"Could not load font!!"<<endl;


  return true;
}
bool RTextRenderer::setText(const char*text, int x, int y, float sf){
  pos.x = x;
  pos.y = y;
  this->size_factor = sf;
  
  SDL_Color color = {255, 255, 255, 0};
  SDL_Surface *surf =
    TTF_RenderText_Blended(this->font, text, color);
  if(tex.id() == 0){
    tex.init(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE,
	     glm::int2(surf->w, surf->h));
    pr->setFlag("ctex" , tex.getUnit());
  }
  else
    tex.resize(surf->w, surf->h);
  this->size = glm::vec2(surf->w/(float)FWIDTH, surf->h/(float)FHEIGHT);
  pr->use();
  glUniform2f(glGetUniformLocation(pr->id(), "size"), size.x, size.y);
  glUniform2f(glGetUniformLocation(pr->id(), "pos"), pos.x, pos.y);
  pr->unbind();
  // float kk[surf->w*surf->h*4];
  // fori(0,surf->w*surf->h*4) kk[i] = rand()/(float)RAND_MAX;
  //  tex.upload(kk);
  tex.upload(surf->pixels);
  SDL_FreeSurface(surf);
  return true;
}

bool RTextRenderer::move(int x, int y){
  pos.x = x;
  pos.y = y;
  pr->use();
  glUniform2f(glGetUniformLocation(pr->id(), "size"), size.x, size.y);
  glUniform2f(glGetUniformLocation(pr->id(), "pos"), pos.x, pos.y);
  pr->unbind();
  return true;
}
void RTextRenderer::draw(){
  pr->setFlag("ctex" , tex.getUnit());
  pr->use();
  glUniform1f(glGetUniformLocation(pr->id(), "fontsize_multiplicator"), size_factor);
  glUniform2f(glGetUniformLocation(pr->id(), "size"), size.x, size.y);
  glUniform2f(glGetUniformLocation(pr->id(), "pos"), pos.x, pos.y);
  glUniform1f(glGetUniformLocation(pr->id(), "FWIDTH"), (float)FWIDTH);
  glUniform1f(glGetUniformLocation(pr->id(), "FHEIGHT"), (float) FHEIGHT);

  dummy_vao->use();
  glDrawArrays(GL_TRIANGLE_STRIP,0,4);
  //dummy_vao.unbind();
  //pr.unbind();
}


//

