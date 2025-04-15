#ifndef RGL_H
#define RGL_H
#include "RShaderProgram.h"
#include "System.h"
#include "glm/gtx/compatibility.hpp"
#include <array>
#include <memory>
#include <vector>

namespace superpunto {

const char *GetGLErrorStr(GLenum err);
bool CheckGLError(std::string msg = "");

struct DataLayout {
  void init(GLint size, GLsizei stride, GLenum type, GLuint offset,
            GLuint divisor = 0, GLboolean normalized = GL_FALSE);
  void init();
  GLsizei size;
  GLsizei stride;
  GLenum type;
  GLboolean normalized;
  GLuint offset;
  GLuint divisor; // instanced
};

class VBO {
public:
  VBO();
  ~VBO();
  VBO(VBO &&other) noexcept;
  VBO &operator=(VBO &&other) noexcept;
  VBO(const VBO &) = delete;
  VBO &operator=(const VBO &) = delete;

  void init(GLenum type, GLbitfield flags, const DataLayout &dl);
  void init(GLenum type, GLbitfield flags);
  void *map(GLenum usage = GL_WRITE_ONLY) const;
  void unmap() const;
  bool initmem(GLenum type, GLbitfield flags, GLsizeiptr size,
               const void *data);
  bool initmem(GLsizeiptr size, const void *data);
  bool upload(GLenum type, GLintptr offset, GLsizeiptr size,
              const void *data) const;
  bool upload(GLintptr offset, GLsizeiptr size, const void *data);

  uint id() const { return this->vid; }
  GLenum type() const { return this->tp; }
  DataLayout get_layout() const { return this->layout; }
  void reset();

  void use() const;
  void unbind() const;

private:
  GLuint vid;
  GLenum tp;
  GLbitfield flags;
  DataLayout layout;
  bool initialized, meminit;
};

class VAO {
public:
  VAO();
  ~VAO();

  // Move support
  VAO(VAO &&other) noexcept;
  VAO &operator=(VAO &&other) noexcept;

  // Optional: disable copy
  VAO(const VAO &) = delete;
  VAO &operator=(const VAO &) = delete;

  uint id() { return this->vid; }
  void set_attrib(uint attrib, const VBO &vbo);
  void use();
  void unbind();

private:
  uint vid;
};

class RTex {
public:
  RTex();
  ~RTex();

  bool upload(const void *data);
  void resize(GLuint wx, GLuint wy);
  void use();
  void unbind();
  GLuint id() { return this->tid; }
  GLuint getUnit() { return this->unit; }
  void init(GLenum ifmt, GLenum efmt, GLenum dtp,
            glm::int2 size); // internal format, external format, data type

  inline glm::int2 getSize() { return this->size; }

  operator GLuint() const { return tid; }

private:
  GLuint tid;
  GLenum tp;
  GLenum format[3]; // internal, external, type
  glm::int2 size;
  GLuint unit;
  static GLuint unit_counter;
};

class FBO {
public:
  FBO(std::shared_ptr<System> sys, glm::int2 resolution);
  ~FBO();
  void use();
  void unbind();
  GLuint id() { return this->fid; }
  void draw();

  glm::u8vec4 getPixel(int x, int y);
  Uint8 *getColorData();

  void setFormat(GLenum ifmt, GLenum efmt, GLenum dtp);

  inline glm::int2 getSize() { return ctex.getSize(); }
  GLuint getTexUnit() { return ctex.getUnit(); }

  void handle_resize(int new_fwidth, int new_fheight);

  RShaderProgram pr;

  glm::int2 getResolution() { return {fwidth, fheight}; }

protected:
  VAO vao; // dummy vao
  GLenum tp;
  GLuint fid;
  RTex ctex; // color

  std::array<GLenum, 3> draw_buffer;
  std::vector<Uint8> cdata;

  std::shared_ptr<System> sys;
  uint fwidth, fheight;
};

class GBuffer : public FBO {
public:
  GBuffer(std::shared_ptr<System> sys, glm::int2 resolution);
  ~GBuffer();
  float *getDepthData();

  GLint getDepthUnit() { return dtex.getUnit(); }
  GLint getPositionUnit() { return ptex.getUnit(); }
  GLint getNormalDepthUnit() { return normdtex.getUnit(); }
  GLint getNoiseUnit() { return noisetex.getUnit(); }
  GLint getColorUnit() { return ctex.getUnit(); }
  void handle_resize(int new_fwidth, int new_fheight);

  RShaderProgram pr;

private:
  RTex dtex, ptex, normdtex; // color, depth, position, normal/linear depth
  RTex noisetex;
  std::vector<float> ddata;
};

class RGLContext {
  std::shared_ptr<System> sys;

public:
  RGLContext(std::shared_ptr<System> sys, SDL_Window *w);
  ~RGLContext();

private:
  void init(SDL_Window *w);
  SDL_GLContext context;
};

} // namespace superpunto
#endif
