#ifndef RGL_H
#define RGL_H
#include "header.h"
#include "RFile.h"
#include<glm/gtx/compatibility.hpp>


const char * GetGLErrorStr(GLenum err);
void CheckGLError();

struct DataLayout{
  void init(GLint size, GLsizei stride, GLenum type, GLuint offset, GLuint divisor = 0, GLboolean normalized = GL_FALSE);
  void init();
  GLsizei size;
  GLsizei stride;
  GLenum type;
  GLboolean normalized;
  GLuint offset;
  GLuint divisor; //instanced
};

class VBO{
 public:
  VBO();
  ~VBO();
  void init(GLenum type, GLbitfield flags, const DataLayout &dl);
  void init(GLenum type, GLbitfield flags);
  void * map(GLenum usage = GL_WRITE_ONLY);
  void unmap();
  bool initmem(GLenum type, GLbitfield flags, GLsizeiptr size, const void *data);
  bool initmem(GLsizeiptr size, const void *data);
  bool upload(GLenum type, GLintptr offset, GLsizeiptr size, const void *data);  
  bool upload(GLintptr offset, GLsizeiptr size, const void *data);

  uint id() const{return this->vid;}  
  GLenum type() const{ return this->tp;}
  DataLayout get_layout() const{return this->layout;}

  void use();
  void unbind();
 private:
  GLuint vid;
  GLenum tp;
  GLbitfield flags;
  DataLayout layout;
  bool initialized, meminit;
};

class VAO{
 public:
  VAO();
  ~VAO();
  uint id(){return this->vid;}
  void set_attrib(uint attrib, const VBO &vbo, GLint binding);
  void use();
  void unbind();
 private:
  uint vid;
};


class RTex{
public:
  RTex();
  ~RTex();

  bool upload(const void *data);
  void resize(GLuint wx, GLuint wy);
  void use();
  void unbind();
  GLuint id(){return this->tid;}
  GLuint getUnit(){return this->unit;}
  void init(GLenum ifmt, GLenum efmt, GLenum dtp); //internal format, external format, data type  
  
  inline glm::int2 getSize(){ return this->size;}

  operator GLuint() const{return tid;} 
private:
  GLuint tid;
  GLenum tp;
  GLenum format[3]; //internal, external, type
  glm::int2 size;
  static GLuint unit;
};


class RShader{
 public:
  RShader();
  ~RShader();
  bool charload(const GLchar* src, GLenum type);
  bool load(const char* fileName, GLenum type);
  GLuint id()const {return this->sid;}
 private:  
  GLenum tp;
  GLuint sid;
};

class RShaderProgram{
 public: 
  RShaderProgram();
  ~RShaderProgram();
  bool init(RShader *shader_list, uint nshaders);
  GLuint id() const{return this->pid;}
  operator GLuint() const{return this->pid;}
  void use();
  void unbind();
 private:
  uint pid;
};

class FBO{
 public:
  FBO();
  ~FBO();
  void use();
  void unbind();
  GLuint id(){return this->fid;}
  void draw();
  Uint8* getColorData();
  float* getDepthData();

  inline glm::int2 getSize(){return ctex.getSize();}

  void handle_resize();
 private:
  GLenum tp;
  GLuint fid;
  RTex ctex, dtex; //color, depth
  GLuint draw_buffer;
  RShaderProgram pr;
  vector<float> ddata;
  vector<Uint8> cdata;

};

class RGLContext{
public:
  RGLContext();
  ~RGLContext();
  void init(SDL_Window *& w);
private:
  SDL_GLContext context;
};

#endif
