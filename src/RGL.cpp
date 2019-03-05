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


RTex::RTex(){  unit_counter++;   this->unit = unit_counter; tid = 0;}
void RTex::init(GLenum ifmt, GLenum efmt, GLenum dtp, glm::int2 size = {FWIDTH, FHEIGHT}){
  tp = GL_TEXTURE_2D;
  format[0] = ifmt;
  format[1] = efmt;
  format[2] = dtp;
  this->size = size;
  if(tid !=0){
    glDeleteTextures(1, &tid);
  }
  glCreateTextures(tp, 1, &tid);
  glBindTextureUnit(unit, tid);
  glTextureStorage2D(tid, 1, format[0], size.x, size.y); //Immutable size  
  //glClearTexSubImage(tid, 0, 0,0,0, size.x,size.y,1, format[1], format[2], 0);
  glTextureParameteri(tid, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTextureParameteri(tid, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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
  init(format[0], format[1], format[2], glm::int2(wx, wy));
  //  glTexImage2D(tp, 0, format[0], size.x, size.y, 0, format[1], format[2], 0);
}

GLuint RTex::unit_counter = -1;


#include"shaders.h"

FBO::FBO(){
  printf("\tInit FBO...     ");
  tp = GL_FRAMEBUFFER;
  glCreateFramebuffers(1, &fid);  

  //Rendering textures/buffers
  draw_buffer = new GLenum[2];
  draw_buffer[0] = GL_COLOR_ATTACHMENT0;
  //Color  texture

  ctex.init(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE); 

  glNamedFramebufferTexture(fid, draw_buffer[0], ctex, 0);

  glNamedFramebufferDrawBuffers(fid, 1, draw_buffer);

  RShader shs[2];
  shs[0].charload(shaders_quad_vs , GL_VERTEX_SHADER);
  shs[1].charload(shaders_quad_fs , GL_FRAGMENT_SHADER);

  pr.init(shs, 2);
  pr.setFlag("ctex", ctex.getUnit());
  printf("DONE!\n");
}

void FBO::setFormat(GLenum ifmt, GLenum efmt, GLenum dtp){
  ctex.init(ifmt, efmt, dtp); 
  glNamedFramebufferTexture(fid, draw_buffer[0], ctex, 0);
}


FBO::~FBO(){
  glDeleteFramebuffers(1, &fid);
  delete[] draw_buffer;
}

void FBO::draw(){
  this->unbind();
  pr.use();
  vao.use();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  vao.unbind();
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
  
  // std::fill(cdata.begin(), cdata.end(), 0);
  // fori(0,FWIDTH*FHEIGHT) cdata[4*i+3] = 255;
  // fori(0,FWIDTH*FHEIGHT) cdata[4*i] = 255;

  return cdata.data();
}

void FBO::handle_resize(){
  ctex.resize(FWIDTH, FHEIGHT);
  glNamedFramebufferTexture(fid, draw_buffer[0], ctex, 0);
}

void FBO::bindColorTex(RShaderProgram &apr){
  apr.setFlag("ctex"    , ctex.getUnit()); 
}

GBuffer::~GBuffer(){
  delete[] draw_buffer;
}
GBuffer::GBuffer(): FBO(){
  printf("\t\tInit GBuffer...     ");
  //Depth texture
  dtex.init(GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT);
  glTextureParameteri(dtex, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTextureParameteri(dtex, GL_TEXTURE_COMPARE_MODE,   GL_NONE);
  glNamedFramebufferTexture(fid, GL_DEPTH_ATTACHMENT, dtex, 0);

  //Rendering textures/buffers
  draw_buffer = new GLenum[3];
  draw_buffer[0] = GL_COLOR_ATTACHMENT0;
  draw_buffer[1] = GL_COLOR_ATTACHMENT1;
  draw_buffer[2] = GL_COLOR_ATTACHMENT2;

  //Normal and linear depth encoded as alpha, for SSAO
  normdtex.init(GL_RGBA32F, GL_RGBA, GL_FLOAT); //Color texture
  glNamedFramebufferTexture(fid, draw_buffer[1], normdtex, 0);

  //Position texture for deferred shading
  ptex.init(GL_RGB16F, GL_RGB, GL_FLOAT); 
  glNamedFramebufferTexture(fid, draw_buffer[2], ptex, 0);

  glNamedFramebufferDrawBuffers(fid, 3, draw_buffer);

  noisetex.init(GL_RGB16F, GL_RGB, GL_FLOAT, glm::int2(4, 4));
  glm::vec3 noise[16];
  
  fori(0,16){
    noise[i] = glm::vec3(RANDESP*2.0-1.0f, RANDESP*2.0-1.0f, 0.0f);
  } 
  noisetex.upload((void *) noise);
  glTextureParameteri(noisetex, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(noisetex, GL_TEXTURE_WRAP_T, GL_REPEAT);  

  //glCheckNamedFrameBufferStatus(fid);
  printf("DONE!\n");
}


float* GBuffer::getDepthData(){
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

void GBuffer::handle_resize(){
  FBO::handle_resize();
  normdtex.resize(FWIDTH, FHEIGHT);
  glNamedFramebufferTexture(fid, draw_buffer[1], normdtex, 0);
  ptex.resize(FWIDTH, FHEIGHT);
  glNamedFramebufferTexture(fid, draw_buffer[2], ptex, 0);
  dtex.resize(FWIDTH, FHEIGHT);
  glNamedFramebufferTexture(fid, GL_DEPTH_ATTACHMENT, dtex, 0);
}

void GBuffer::bindSamplers(RShaderProgram &apr){
  apr.setFlag("ctex"    , ctex.getUnit());     
  apr.setFlag("dtex"    , dtex.getUnit());     
  apr.setFlag("ndtex"   , normdtex.getUnit()); 
  apr.setFlag("ptex"    , ptex.getUnit());     
  apr.setFlag("noisetex", noisetex.getUnit()); 
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
  int bufflen;
  glGetShaderiv(sid, GL_INFO_LOG_LENGTH, &bufflen);
  if (bufflen > 1){
    GLchar* log_string = new char[bufflen + 1];
    glGetShaderInfoLog(sid, bufflen, 0, log_string);
    printf("%s\n", log_string);
    
    delete log_string;
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
  int isLinked = 0;
  glGetProgramiv(pid, GL_LINK_STATUS, (int *)&isLinked);
  if(isLinked == GL_FALSE){
    int bl = 0;
    glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &bl);
    std::vector<GLchar> infoLog(bl);
    glGetProgramInfoLog(pid, bl, &bl, &infoLog[0]);
    printf("%s\n",infoLog.data());

  }
  return true;
}

void RShaderProgram::use(){glUseProgram(pid);}
void RShaderProgram::unbind(){glUseProgram(0);}

void RShaderProgram::setFlag(const GLchar* flag, int val){
  glProgramUniform1i(pid, glGetUniformLocation(pid, flag), val);
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
  //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,          8);
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
  
  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  //glEnable(GL_FRAMEBUFFER_SRGB); 
   glEnable(GL_MULTISAMPLE);
   glEnable(GL_LINE_SMOOTH);
   glEnable(GL_POLYGON_SMOOTH);
   
}








