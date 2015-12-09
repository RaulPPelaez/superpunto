#ifndef GLIB_H
#define GLIB_H

#include "defines.h"
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<sstream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <SFML/Graphics.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



using namespace std;

#include<cmath>

enum ROPTIONS{
  RGL_POSTPROCESS = 1 << 0,
  RGL_SHADOWMAP = 1 << 1
};


void fillCircle(double r, double x, double y);
void drawCircle(double r, double x, double y);
void drawRect(float x1, float y1, float x2, float y2);
void fillRect(float x1, float y1, float x2, float y2);


GLuint createShader(GLenum type, const GLchar* src);
void generate_vbo_cube(GLuint &posVBO, GLuint &normalsVBO, GLuint &indicesVBO);
void generate_sphere_vbos(GLuint &posVBO, GLuint &indicesVBO);
GLfloat* get_wired_cube();
static char* textFileRead(const char *fileName);

class RShader{
public:
   RShader(){}
   ~RShader();
   bool load(const char *fileName, GLint type);
   bool charload(const GLchar *shader, GLint type);
   GLuint id(){ return this->sh;}
   GLint get_type(){return this->type;}
private:
   GLuint sh; // ID of shader
   GLint type; // GL_VERTEX_SHADER, GL_FRAGMENT_SHADER...
};


class RShaderProgram{
public:
   RShaderProgram(){}
   ~RShaderProgram(){glDeleteProgram(pr);}
   void create(){this->pr = glCreateProgram();}
   void initialize(RShader vs, RShader fs){
     create();
     add_shader(vs);
     add_shader(fs);
     link();
     use();

   }
   void add_shader(RShader sh){
     glAttachShader(pr, sh.id());
     if(sh.get_type() == GL_VERTEX_SHADER) this->vs = sh;
     else this->fs = sh;
   }
   GLuint set_attrib(const GLchar * name, GLint size, GLsizei stride, const GLvoid *pointer){
     GLint attrib = glGetAttribLocation(pr, name);
     glEnableVertexAttribArray(attrib);
     glVertexAttribPointer(attrib, size, GL_FLOAT, GL_FALSE, stride, pointer);

     return attrib;
   }   
   GLuint set_attrib_instanced(const GLchar * name, GLint size, GLsizei stride, const GLvoid *pointer){
     GLuint attrib = glGetAttribLocation(pr, name);
     glEnableVertexAttribArray(attrib);
     glVertexAttribPointer(attrib, size, GL_FLOAT, GL_FALSE, stride, pointer);
     glVertexAttribDivisor( attrib, 1); //is it instanced?

     return attrib;
   }
   
   GLuint get_attrib_handle(const GLchar * name){
     return glGetUniformLocation(pr, name);
   }

   void link(){glLinkProgram (pr);}
   void use(){if(!in_use)glUseProgram(pr); in_use = true;}
   void unbind(){glUseProgram(0); in_use = false;}
   bool is_in_use(){ return this->in_use;}
   GLuint id(){return this->pr;}
private:
   GLuint pr; // ID of program
   bool in_use;
   RShader vs, fs;
};


class RTexture{
public:
  RTexture(){}
  RTexture(const char *fileName){ this->Load(fileName); }
  ~RTexture(){ /*glDeleteTextures(1,&texID);*/}
  void Load(const char *fileName){
    glGenTextures(1,&texID);
    sf::Image image;
    if(!image.loadFromFile(fileName)) printf("ERROR! Texture image not found!\n");
    
    glGenSamplers(1, &samplerID);
    glActiveTexture(GL_TEXTURE0+samplerID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
  }
  void Load(const char *fileName, GLint param){
    glGenTextures(1,&texID);
    sf::Image image;
    if(!image.loadFromFile(fileName)) printf("ERROR! Texture image not found!\n");
    
    glGenSamplers(1, &samplerID);
    glActiveTexture(GL_TEXTURE0+samplerID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
  }
  GLuint* id(){return &(this->texID);}
  GLuint sampler_id(){return this->samplerID;}
  void Bind(){
    glBindTexture(GL_TEXTURE_2D, texID);
  }
  
private:
  GLuint texID, samplerID;

};


const char * GetGLErrorStr(GLenum err);
void CheckGLError();


#endif
