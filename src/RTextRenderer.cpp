#include"RTextRenderer.h"

bool RTextRenderer::SDL_ttf_init = false;
RTextRenderer::RTextRenderer():
  font(nullptr),
  size(glm::vec2(0)),
  pos(glm::vec2(0)){};

bool RTextRenderer::setFont(const char * fontName, int size){
  if(!SDL_ttf_init){
    TTF_Init();
    RShader shs[2];
    shs[0].load("../src/shaders/text.vs", GL_VERTEX_SHADER);
    shs[1].load("../src/shaders/text.fs", GL_FRAGMENT_SHADER);
    pr.init(shs, 2);
    SDL_ttf_init = true;
  }
  this->font = TTF_OpenFont(fontName, size);
  if(!font) cerr<<"Could not load font!!"<<endl;


  return true;
}
bool RTextRenderer::setText(const char*text, int x, int y){
  pos.x = x;
  pos.y = y;
  
  SDL_Color color = {255, 255, 255, 0};
  SDL_Surface *surf =
    TTF_RenderText_Blended(this->font, text, color);
  if(tex.id() == 0){
    tex.init(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE,
	     glm::int2(surf->w, surf->h));
    pr.setFlag("ctex" , tex.getUnit());
  }
  else
    tex.resize(surf->w, surf->h);
  this->size = glm::vec2(surf->w/(float)FWIDTH, surf->h/(float)FHEIGHT);
  pr.use();
  glUniform2f(glGetUniformLocation(pr.id(), "size"), size.x, size.y);
  glUniform2f(glGetUniformLocation(pr.id(), "pos"), pos.x, pos.y);
  pr.unbind();
  // float kk[surf->w*surf->h*4];
  // fori(0,surf->w*surf->h*4) kk[i] = rand()/(float)RAND_MAX;
  //  tex.upload(kk);
  tex.upload(surf->pixels);
  SDL_FreeSurface(surf);
}

void RTextRenderer::draw(){
  pr.use();
  dummy_vao.use();
  glDrawArrays(GL_TRIANGLE_STRIP,0,4);
  dummy_vao.unbind();
  pr.unbind();

}


//

