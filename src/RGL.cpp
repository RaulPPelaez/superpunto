
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
  glGenBuffers(1, &vid);
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
  this->use();
  glBufferStorage(this->tp, size, data, this->flags);
  this->unbind();
  meminit = true;
  return true;
}
bool VBO::upload(GLenum type, GLintptr offset, GLsizeiptr size, const void *data){
  glBufferSubData(type, offset, size, data);
  return true;
}
bool VBO::upload(GLintptr offset, GLsizeiptr size, const void *data){
  if(!this->meminit) return false;
  this->use();
  glBufferSubData(this->tp, offset, size, data);
  this->unbind();
  // glNamedBufferSubData(vid, offset, size, data);
  return true;
}
void VBO::use() const{ glBindBuffer(tp, vid);}
void VBO::unbind() const{ glBindBuffer(tp, 0);}




VAO::VAO(){glGenVertexArrays(1,&vid);}
VAO::~VAO(){glDeleteVertexArrays(1,&vid);}
void VAO::set_attrib(uint attrib, const VBO &vbo, GLint binding){
  if(vbo.type() == GL_ELEMENT_ARRAY_BUFFER) return;
  DataLayout dl = vbo.get_layout();
  this->use();
  vbo.use();
  //  glVertexArrayAttribFormat(vid, attrib, dl.size, dl.type, dl.normalized, dl.offset);
  // glVertexArrayAttribBinding(vid, attrib, binding);
  // glVertexArrayVertexBuffer(vid, binding, vbo.id(), 0, dl.stride);

  glVertexAttribPointer(attrib, dl.size, dl.type, dl.normalized,  dl.stride,  0);


  glEnableVertexAttribArray(attrib);



  glVertexAttribDivisor(attrib, dl.divisor);
  this->unbind();
  vbo.unbind();

}
void VAO::use(){glBindVertexArray(vid);}
void VAO::unbind(){glBindVertexArray(0);}




FBO::FBO(){ glGenFramebuffers(1, &fid);}
FBO::~FBO(){ glDeleteFramebuffers(1, &fid);}



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





RGLContext::RGLContext(){  }
RGLContext::~RGLContext(){ SDL_GL_DeleteContext(context); }
void RGLContext::init(SDL_Window *& w){
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

  context = SDL_GL_CreateContext(w);
  
  if(SDL_GL_SetSwapInterval(-1)<0) SDL_GL_SetSwapInterval(2);

  SDL_Delay(100);

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


