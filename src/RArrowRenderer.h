#ifndef RARROWRENDERER_H
#define RARROWRENDERER_H
#include "RRenderer.h"
#include<map>
class RArrowRenderer: public RRenderer{
public:
  RArrowRenderer(int maxN, float gscale, RConfig cfg);
  ~RArrowRenderer(){}

  void update() override;
  void draw() override;

  bool upload_instances(ParticleData pdata) override;

  void handle_event(SDL_Event &e) override;

private:
  bool init_buffers();
  bool init_shaders();
  bool init_uniforms();
  
  int maxN;

  RShaderProgram pr;
  
  VAO vao;
  VBO arrow_vbos[3]; //pos, dir/length, color
  std::map<string, uint> attribs;

  float dirscale;
  
  int uniMVP, unimodel, uniproj, uniview, unicam;

};


#endif
