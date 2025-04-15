#include "RBox.h"

#include"shaders.h"

namespace superpunto{
  RBox::RBox(float gscale, glm::mat4 *MVP, glm::vec3 size):
    gscale(gscale),
    MVP(MVP),
    size(size)
  {
    DataLayout dl;
    dl.init(3, 3*sizeof(float), GL_FLOAT, 0);
    vbo.init(GL_ARRAY_BUFFER, GL_MAP_READ_BIT, dl);
    fill_box_vbo(vbo);

    vao.set_attrib(0, vbo);

    RShader shs[2];
    shs[0].charload(shaders_box_vs, GL_VERTEX_SHADER);
    shs[1].charload(shaders_box_fs, GL_FRAGMENT_SHADER);
    pr.init(shs, 2);

    pr.use();
    glUniform1f(glGetUniformLocation(pr.id(), "gscale"), this->gscale);
    glUniform3f(glGetUniformLocation(pr.id(), "color"), 0,0,1);
    pr.unbind();


  };

  void RBox::setSize(glm::vec3 L){

    this->size = L;
    pr.use();
    glUniform3f(glGetUniformLocation(pr.id(), "boxSize"), 2*size.x, 2*size.y, 2*size.z);
    pr.unbind();
  }


  void RBox::draw(){
    pr.use();
    vao.use();
    glUniformMatrix4fv(glGetUniformLocation(pr.id(), "MVP"), 1, GL_FALSE, glm::value_ptr(*MVP));
    glLineWidth(3.0f);
    glDrawArrays(GL_LINES, 0, 72);
    glLineWidth(1.0f);
    vao.unbind();
    pr.unbind();
  };

  void fill_box_vbo(VBO &boxVBO){
    float v[] = {
      0,0,0, 0,1,0,
      1,0,0, 1,1,0,
      1,0,1, 1,1,1,
      0,0,1, 0,1,1,
      0,1,0, 0,1,1,
      1,1,0, 1,1,1,
      1,0,0, 1,0,1,
      0,0,0, 0,0,1,
      0,1,0, 1,1,0,
      0,1,1, 1,1,1,
      0,0,1, 1,0,1,
      0,0,0, 1,0,0
    };

    fori(0,72) v[i] -= 0.5f;

    boxVBO.initmem(sizeof(v), v);
  }
}
