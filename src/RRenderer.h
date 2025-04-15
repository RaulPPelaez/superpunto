#ifndef RRENDERER_H
#define RRENDERER_H

#include"RFile.h"
#include"System.h"
#include"RGL.h"
#include"RTextRenderer.h"
#include"RBox.h"
#include"Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"RWindow.h"


namespace superpunto{
  class RAxis{
  public:
    RAxis(std::shared_ptr<System> sys, std::shared_ptr<RWindow> w);
    void draw(const glm::mat4& viewMatrix, const glm::mat4& model, const glm::ivec2& resolution);


  private:
    RTextRenderer Xtext, Ytext, Ztext;
    RShaderProgram pr;
    glm::mat4 *MVP;
    float axislength;
    glm::vec3 *campos;
    glm::vec3 origin;
    VAO dummy_vao, vao;
    VBO vbo;
    std::shared_ptr<System> sys;
    std::shared_ptr<RWindow> w;

  };



  class RRenderer{
  public:
    using Camera = FreeCamera;
  protected:

    std::shared_ptr<System> sys;
    std::shared_ptr<Camera> cam;
    float gscale;
    RBox box;
    RAxis axis;
    glm::mat4 MVP, model, view, proj;
    RTextRenderer textRenderer;
    ParticleData particles; //Current particle data in CPU
    std::shared_ptr<RWindow> w;

  public:
    RRenderer(std::shared_ptr<System> sys, std::shared_ptr<RWindow> w, std::shared_ptr<Camera> cam, float gscale);

    virtual void handle_event(SDL_Event &e);

    virtual void update();
    virtual void draw() = 0;
    void display();
    virtual void drawText(const char* text, int x, int y);
    virtual void handle_resize(uint fw, uint fh);
    void rotate_model(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
    void reset_model();

    virtual void upload_instances(ParticleData pdata) = 0;

    virtual int pick(int x, int y, int pickindex);

    virtual Uint8 *getPixels();
    virtual glm::int2 getSize();

    int picked[2];
    float znear = 0.1;
    float zfar = 10000;
  };



}

#endif
