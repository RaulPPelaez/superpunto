#ifndef RGL_H
#define RGL_H
#include "defines.h"
#include "RFile.h"
#include"System.h"
#include"glm/gtx/compatibility.hpp"

#include <vector>
#include<array>

namespace superpunto{

  const char * GetGLErrorStr(GLenum err);
  bool CheckGLError(std::string msg = "");

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
    void reset();
    
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
    void init(GLenum ifmt, GLenum efmt, GLenum dtp, glm::int2 size); //internal format, external format, data type  
  
    inline glm::int2 getSize(){ return this->size;}

    operator GLuint() const{return tid;} 
  private:
    GLuint tid;
    GLenum tp;
    GLenum format[3]; //internal, external, type
    glm::int2 size;
    GLuint unit;
    static GLuint unit_counter;
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
    void setFlag(const GLchar* flag, int val);
    operator GLuint() const{return this->pid;}
    void use();
    void unbind();
  private:
    uint pid;
  };


  class FBO{
  public:
    FBO(std::shared_ptr<System> sys, glm::int2 resolution);
    ~FBO();
    void use();
    void unbind();
    GLuint id(){return this->fid;}
    void draw();

    glm::vec4 getPixel(int x, int y);
    Uint8* getColorData();
  
    void setFormat(GLenum ifmt, GLenum efmt, GLenum dtp);

    inline glm::int2 getSize(){return ctex.getSize();}
    void bindColorTex(RShaderProgram &apr);
    GLuint getTexUnit(){return ctex.getUnit();}

    void handle_resize(int new_fwidth, int new_fheight);

    RShaderProgram pr;

    glm::int2 getResolution(){ return {fwidth, fheight};}
  protected:
    VAO vao; //dummy vao
    GLenum tp;
    GLuint fid;
    RTex ctex; //color

    std::array<GLenum,3> draw_buffer;
    std::vector<Uint8> cdata;
    
    std::shared_ptr<System> sys;
    uint fwidth, fheight;

  };



  class GBuffer: public FBO{
  public:
    GBuffer(std::shared_ptr<System> sys, glm::int2 resolution);
    ~GBuffer();
    float* getDepthData();

    void bindSamplers(RShaderProgram &apr);

    void handle_resize(int new_fwidth, int new_fheight);

    RShaderProgram pr;
  private:
    RTex dtex, ptex, normdtex; //color, depth, position, normal/linear depth 
    RTex noisetex;
    std::vector<float> ddata;


  

  };

  class RGLContext{
    std::shared_ptr<System> sys;
  public:
    RGLContext(std::shared_ptr<System> sys, SDL_Window * w);
    ~RGLContext();

  private:
    void init(SDL_Window * w);
    SDL_GLContext context;
  };

}
#endif
