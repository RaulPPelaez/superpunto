#include "glib.h"




void fillCircle(double r, double x, double y){
      
       int n = PI2*r*10;
       double s = sin(PI2/n);
       double c = cos(PI2/n);
       double dx = r;
       double dy = 0.0;
       double dxtemp;
       glBegin(GL_TRIANGLE_FAN);
	 glVertex2d(x,y);

	 fori(1,n+3){
	   glVertex2d(x+dx,y+dy);
	   dxtemp = dx;
	   dx = (dx*c - dy*s);
	   dy = (dy*c + dxtemp*s);
	   
	   //dx, dy = (dx*c - dy*s), (dy*c + dx*s);
	 }
   
      glEnd();

}
void drawCircle(double r, double x, double y){
      
       int n = PI2*r;
       double s = sin(PI2/n);
       double c = cos(PI2/n);
       double dx = r;
       double dy = 0.0;
       double dxtemp;
       glBegin(GL_LINE_STRIP);
	 //glVertex2d(x,y);

	 fori(1,n+2){
	   
	   glVertex2d(x+dx,y+dy);

	   dxtemp = dx;
	   dx = (dx*c - dy*s);
	   dy = (dy*c + dxtemp*s);
	   
	   //dx, dy = (dx*c - dy*s), (dy*c + dx*s);
	 }
   
      glEnd();

}
void fillRect(float x1, float y1, float x2, float y2){
  glBegin(GL_QUADS);
    glNormal3d(0,0,1);
    glTexCoord2f(0,0);glVertex3f(x1, y1, 0.0f); 
    glTexCoord2f(0, 1);glVertex3f(x2, y1, 0.0f);    
    glTexCoord2f(1, 1);glVertex3f(x2, y2, 0.0f);   
    glTexCoord2f(1, 0);glVertex3f(x1, y2, 0.0f);     
  glEnd();
}
void drawRect(float x1, float y1, float x2, float y2){
  glBegin(GL_QUADS);
    glNormal3d(0,0,1);
    glVertex3f(x1, y1, 0.0f); 
    glVertex3f(x2, y1, 0.0f);    
    glVertex3f(x2, y2, 0.0f);   
    glVertex3f(x1, y2, 0.0f);     
  glEnd();
}

GLuint generate_vbo_cube(){
  GLfloat vertices[] = {
    // X      Y     Z     R     G     B     U     V
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    
    -1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
     1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
     1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
  };
  GLuint vbo = 0;
  glGenBuffers (1, &vbo);
  glBindBuffer (GL_ARRAY_BUFFER, vbo);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);
  return vbo;
}



bool RShader::load(const char *fileName, GLint type){
  this-> type = type;
  sh = glCreateShader(this->type);
  const GLchar* src = textFileRead(fileName);
  glShaderSource(sh, 1, &src, nullptr);
  glCompileShader(sh);
  delete[] src;
  int iCompilationStatus;
  glGetShaderiv(sh, GL_COMPILE_STATUS, &iCompilationStatus);
  if(iCompilationStatus == GL_FALSE){
    std::cout << "Could not compile shader: " << std::endl;
           char buffer[512];
        glGetShaderInfoLog(sh, 512, NULL , buffer);
        std::cout << buffer << std::endl;
	    return false;}
  return true;
}
RShader::~RShader(){glDeleteShader(sh);}

static char* textFileRead(const char *fileName) {
    char* text;
    
    if (fileName != NULL) {
        FILE *file = fopen(fileName, "rt");
        
        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);
            
            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';
            }
            fclose(file);
        }
    }
    return text;
}


GLuint createShader(GLenum type, const GLchar* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    return shader;
}

