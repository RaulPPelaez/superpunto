
#include "RGL.h"


const char * GetGLErrorStr(GLenum err){
  switch (err){                                           
  case GL_NO_ERROR:          return "No error";         
  case GL_INVALID_ENUM:      return "Invalid enum";     
  case GL_INVALID_VALUE:     return "Invalid value";    
  case GL_INVALID_OPERATION: return "Invalid operation";
  case GL_STACK_OVERFLOW:    return "Stack overflow";   
  case GL_STACK_UNDERFLOW:   return "Stack underflow";  
  case GL_OUT_OF_MEMORY:     return "Out of memory";    
  default:                   return "Unknown error";    
  }                                                                                          
}
void CheckGLError(){
  while (true){
    const GLenum err = glGetError();
    if (GL_NO_ERROR == err)break;
    std::cout << "GL Error: " << GetGLErrorStr(err) << std::endl;
  }
}


void DataLayout::init(GLsizei size, GLsizei stride, GLenum type, GLuint offset, GLuint divisor, GLboolean normalized){
  this->size=size;
  this->stride=stride;
  this->type=type;
  this->offset=offset;
  this->divisor=divisor;
  this->normalized=normalized;
}
void DataLayout::init(){
  init(0,0,GL_UNSIGNED_INT,0);
}




VBO::VBO(){
  glCreateBuffers(1, &vid);
  initialized = meminit = false;
}
VBO::~VBO(){glDeleteBuffers(1, &vid);}
void VBO::init(GLenum type, GLbitfield flags, const DataLayout &dl){
  this->tp = type;
  this->flags = flags;
  initialized = true;
  layout = dl;
}

void VBO::init(GLenum type, GLbitfield flags){
  this->tp = type;
  this->flags = flags;
  initialized = true;
  layout.init();
}


bool VBO::initmem(GLenum type, GLbitfield flags, GLsizeiptr size, const void *data){
  glBufferStorage(type, size, data, flags);
  return true;
}
bool VBO::initmem(GLsizeiptr size, const void *data){
  if(!this->initialized) return false;
  glNamedBufferStorage(this->vid, size, data, this->flags);
  meminit = true;
  return true;
}
bool VBO::upload(GLenum type, GLintptr offset, GLsizeiptr size, const void *data){
  glBufferSubData(type, offset, size, data);
  return true;
}
bool VBO::upload(GLintptr offset, GLsizeiptr size, const void *data){
  if(!this->meminit) return false;
  glNamedBufferSubData(vid, offset, size, data);
  return true;
}


void *VBO::map(GLenum usage){ return glMapNamedBuffer(vid, usage);}
void VBO::unmap(){ glUnmapNamedBuffer(vid);}

void VBO::use(){ glBindBuffer(tp, vid);}
void VBO::unbind(){ glBindBuffer(tp, 0);}




VAO::VAO(){glCreateVertexArrays(1,&vid);}
VAO::~VAO(){glDeleteVertexArrays(1,&vid);}

void VAO::set_attrib(uint attrib, const VBO &vbo, GLint binding){
  if(vbo.type() == GL_ELEMENT_ARRAY_BUFFER) return;
  DataLayout dl = vbo.get_layout();
  glVertexArrayAttribFormat(vid, attrib, dl.size, dl.type, dl.normalized, dl.offset);
  glVertexArrayAttribBinding(vid, attrib, binding);
  glVertexArrayVertexBuffer(vid, binding, vbo.id(), 0, dl.stride);
  glEnableVertexArrayAttrib(vid, attrib);
  this->use();
  glVertexAttribDivisor(attrib, dl.divisor);
  this->unbind();

}
void VAO::use(){glBindVertexArray(vid);}
void VAO::unbind(){glBindVertexArray(0);}


RTex::RTex(){}
void RTex::init(GLenum ifmt, GLenum efmt, GLenum dtp){
  unit++;
  tp = GL_TEXTURE_2D;
  format[0] = ifmt;
  format[1] = efmt;
  format[2] = dtp;

  size = {FWIDTH, FHEIGHT};

  glCreateTextures(tp, 1, &tid);
  glBindTextureUnit(unit, tid);
  //glActiveTexture(GL_TEXTURE0+unit);
  glTextureStorage2D(tid, 1, format[0], size.x, size.y); //Immutable size  
  // glBindTexture(tp, tid);
  // glTexImage2D(tp, 0, format[0], size.x, size.y, 0, format[1], format[2], 0);
  // glBindTexture(tp, 0);
  //glTextureParameteri(tid, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  //glTextureParameteri(tid, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //glTextureParameteri(tid, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  //glTextureParameteri(tid, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  //glTextureSubImage2D(texid, 0, 0,0, FWIDTH, FHEIGHT, GL_RGBA, 0);
  //Use if teximage is used
  glClearTexSubImage(tid, 0, 0,0,0, size.x,size.y,1, format[1], format[2], 0);

}

RTex::~RTex(){glDeleteTextures(1, &tid);}
void RTex::use(){
  //glBindTextureUnit(unit, tid);
  glBindTexture(tp, tid);
}
void RTex::unbind(){
  glBindTexture(tp, 0);
  //  glBindTextureUnit(unit, 0);
}

bool RTex::upload(const void *data){
  glTextureSubImage2D(tid, 0, 0,0, size.x, size.y, format[1], format[2], data);

  return true;
}
void RTex::resize(GLuint wx, GLuint wy){
  //size.x = wx;
  //size.y = wy;
  glDeleteTextures(1, &tid);
  init(format[0], format[1], format[2]);
  //  glTexImage2D(tp, 0, format[0], size.x, size.y, 0, format[1], format[2], 0);
}

GLuint RTex::unit = -1;


#include"shaders.h"

FBO::FBO(){ 
  printf("\tInit FBO...     ");
  tp = GL_FRAMEBUFFER;
  glCreateFramebuffers(1, &fid);  

  //Depth texture
  dtex.init(GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT);
  glTextureParameteri(dtex, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTextureParameteri(dtex, GL_TEXTURE_COMPARE_MODE,   GL_NONE);
  glNamedFramebufferTexture(fid, GL_DEPTH_ATTACHMENT, dtex, 0);


  draw_buffer = GL_COLOR_ATTACHMENT0;
  ctex.init(GL_RGBA32F, GL_RGBA, GL_UNSIGNED_BYTE); //Color texture
  glNamedFramebufferTexture(fid, draw_buffer, ctex, 0);

  glNamedFramebufferDrawBuffers(fid, 1, &draw_buffer);

  RShader shs[2];
  shs[0].charload(VS_QUAD_SOURCE, GL_VERTEX_SHADER);
  shs[1].charload(FS_QUAD_SOURCE, GL_FRAGMENT_SHADER);
  pr.init(shs, 2);

  printf("DONE!\n");
}
FBO::~FBO(){ glDeleteFramebuffers(1, &fid);}

void FBO::draw(){
  glDisable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT);
  pr.use();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  pr.unbind();
}
void FBO::use(){ glBindFramebuffer(tp, fid);}
void FBO::unbind(){ glBindFramebuffer(tp, 0);}

glm::vec4 FBO::getPixel(int x, int y){
  //Origin equal to the one fiven by SDL mouse pos, not sure wich corner
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fid);
  Uint8 p[4];
  glReadPixels(x,FHEIGHT-y, 1,1, GL_RGBA, GL_UNSIGNED_BYTE, (void *)p);
  return {p[0], p[1], p[2], p[3]};
}

Uint8* FBO::getColorData(){
  int cdatasize = ctex.getSize().x*ctex.getSize().y*4;  
  if(cdata.size() != cdatasize) cdata.resize(cdatasize);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fid);
  glReadPixels(0,0, FWIDTH,FHEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, (void *)cdata.data());
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  return cdata.data();
}


float* FBO::getDepthData(){
  int ddatasize = dtex.getSize().x*dtex.getSize().y;  
  if(ddata.size() != ddatasize) ddata.resize(ddatasize);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fid);
  glReadPixels(0,0, FWIDTH,FHEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT, (void *)ddata.data());
  // ddata2.resize(ddatasize*4);
  // fori(0,ddatasize){ 
  //   ddata2[4*i] = int(255*ddata[i]);
  //   ddata2[4*i+1] = int(255*ddata[i]);
  //   ddata2[4*i+2] = int(255*ddata[i]);
  //   ddata2[4*i+3] = 255;
  // }
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  return ddata.data();
}

void FBO::handle_resize(){
  ctex.resize(FWIDTH, FHEIGHT);
  glNamedFramebufferTexture(fid, draw_buffer, ctex, 0);
  dtex.resize(FWIDTH, FHEIGHT);
  glNamedFramebufferTexture(fid, GL_DEPTH_ATTACHMENT, dtex, 0);
}


RShader::RShader(){}
RShader::~RShader(){glDeleteShader(sid);}


bool RShader::charload(const GLchar * src, GLenum type){
  if(!src) return false;
  tp = type;
  sid = glCreateShader(type);
  glShaderSource(sid, 1, &src, NULL);
  glCompileShader(sid);
  int iCompilationStatus;
  glGetShaderiv(sid, GL_COMPILE_STATUS, &iCompilationStatus);
  if(iCompilationStatus == GL_FALSE){
    printf("Could not compile shader:\n");
    char buffer[512];
    glGetShaderInfoLog(sid, 512, NULL , buffer);
    printf("%s\n", buffer);
    return false;
  }
  return true;
}

bool RShader::load(const char * fileName, GLenum type){
  return charload(read_file(fileName).c_str(), type);
}

RShaderProgram::RShaderProgram(){pid = glCreateProgram();}
RShaderProgram::~RShaderProgram(){glDeleteProgram(pid);}

bool RShaderProgram::init(RShader *shader_list, uint nshaders){
  fori(0,nshaders) glAttachShader(pid, shader_list[i].id());
  glLinkProgram(pid);
  return true;
}

void RShaderProgram::use(){glUseProgram(pid);}
void RShaderProgram::unbind(){glUseProgram(0);}

void RShaderProgram::setFlag(const GLchar* flag, int val){
  use();
  glUniform1i(glGetUniformLocation(pid, flag), val);
}

RGLContext::RGLContext(){  }
RGLContext::~RGLContext(){ SDL_GL_DeleteContext(context); }

void RGLContext::init(SDL_Window *& w){
  //Setup SDL
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,    1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,          1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,           24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,          8);
  //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

  context = SDL_GL_CreateContext(w);

  if(SDL_GL_SetSwapInterval(-1)<0) SDL_GL_SetSwapInterval(1);

  //Setup OpenGL
  // Init GLEW
  glewExperimental = GL_TRUE;
  glewInit();
  int major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);

  printf("\tOpenGL version %d.%d available!\n", major, minor);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  //glEnable(GL_MULTISAMPLE);
}


