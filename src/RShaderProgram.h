#pragma once
#include "System.h"
namespace superpunto {
class RShader {
public:
  RShader();
  ~RShader();
  bool charload(const GLchar *src, GLenum type);
  bool load(const char *fileName, GLenum type);
  GLuint id() const { return this->sid; }

private:
  GLenum tp;
  GLuint sid;
};

class RShaderProgram {
public:
  RShaderProgram();
  ~RShaderProgram();
  bool init(RShader *shader_list, uint nshaders);
  GLuint id() const { return this->pid; }
  template <typename T>
  void setUniform(const GLchar *flag, const T *val, int count);
  template <typename T> void setUniform(const GLchar *flag, const T &val) {
    setUniform(flag, &val, 1);
  }

  operator GLuint() const { return this->pid; }
  void use();
  void unbind();

private:
  GLuint pid;
};
} // namespace superpunto
