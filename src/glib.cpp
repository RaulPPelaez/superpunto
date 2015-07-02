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

void generate_sphere_vbos(GLuint &posVBO, GLuint &indicesVBO){
  
  GLfloat temp1[] = {
  0.0000000,      -1.0000000,     0.0000000,
0.4253230,      -0.8506540,     0.3090110,
-0.1624560,     -0.8506540,     0.4999950,
0.7236070,      -0.4472200,     0.5257250,
0.8506480,      -0.5257360,     0.0000000,
-0.5257300,     -0.8506520,     0.0000000,
-0.1624560,     -0.8506540,     -0.4999950,
0.4253230,      -0.8506540,     -0.3090110,
0.9510580,      0.0000000,      0.3090130,
-0.2763880,     -0.4472200,     0.8506490,
0.2628690,      -0.5257380,     0.8090120,
0.0000000,      0.0000000,      1.0000000,
-0.8944260,     -0.4472160,     0.0000000,
-0.6881890,     -0.5257360,     0.4999970,
-0.9510580,     0.0000000,      0.3090130,
-0.2763880,     -0.4472200,     -0.8506490,
-0.6881890,     -0.5257360,     -0.4999970,
-0.5877860,     0.0000000,      -0.8090170,
0.7236070,      -0.4472200,     -0.5257250,
0.2628690,      -0.5257380,     -0.8090120,
0.5877860,      0.0000000,      -0.8090170,
0.5877860,      0.0000000,      0.8090170,
-0.5877860,     0.0000000,      0.8090170,
-0.9510580,     0.0000000,      -0.3090130,
0.0000000,      0.0000000,      -1.0000000,
0.9510580,      0.0000000,      -0.3090130,
0.2763880,      0.4472200,      0.8506490,
0.6881890,      0.5257360,      0.4999970,
0.1624560,      0.8506540,      0.4999950,
-0.7236070,     0.4472200,      0.5257250,
-0.2628690,     0.5257380,      0.8090120,
-0.4253230,     0.8506540,      0.3090110,
-0.7236070,     0.4472200,      -0.5257250,
-0.8506480,     0.5257360,      0.0000000,
-0.4253230,     0.8506540,      -0.3090110,
0.2763880,      0.4472200,      -0.8506490,
-0.2628690,     0.5257380,      -0.8090120,
0.1624560,      0.8506540,      -0.4999950,
0.8944260,      0.4472160,      0.0000000,
0.6881890,      0.5257360,      -0.4999970,
0.5257300,      0.8506520,      0.0000000,
0.0000000,      1.0000000,      0.0000000
  };
  
  GLuint temp2[] = {
    0,
    1,
    2,
    3,
    1,
    4,
    0,
    2,
    5,
    0,
    5,
    6,
    0,
    6,
    7,
    3,
    4,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16,
    17,
    18,
    19,
    20,
    3,
    8,
    21,
    9,
    11,
    22,
    12,
    14,
    23,
    15,
    17,
    24,
    18,
    20,
    25,
    26,
    27,
    28,
    29,
    30,
    31,
    32,
    33,
    34,
    35,
    36,
    37,
    38,
    39,
    40,
    2,
    10,
    9,
    2,
    1,
    10,
    1,
    3,
    10,
    4,
    7,
    18,
    4,
    1,
    7,
    1,
    0,
    7,
    5,
    13,
    12,
    5,
    2,
    13,
    2,
    9,
    13,
    6,
    16,
    15,
    6,
    5,
    16,
    5,
    12,
    16,
    7,
    19,
    18,
    7,
    6,
    19,
    6,
    15,
    19,
    8,
    25,
    38,
    8,
    4,
    25,
    4,
    18,
    25,
    11,
    21,
    26,
    11,
    10,
    21,
    10,
    3,
    21,
    14,
    22,
    29,
    14,
    13,
    22,
    13,
    9,
    22,
    17,
    23,
    32,
    17,
    16,
    23,
    16,
    12,
    23,
    20,
    24,
    35,
    20,
    19,
    24,
    19,
    15,
    24,
    21,
    27,
    26,
    21,
    8,
    27,
    8,
    38,
    27,
    22,
    30,
    29,
    22,
    11,
    30,
    11,
    26,
    30,
    23,
    33,
    32,
    23,
    14,
    33,
    14,
    29,
    33,
    24,
    36,
    35,
    24,
    17,
    36,
    17,
    32,
    36,
    25,
    39,
    38,
    25,
    20,
    39,
    20,
    35,
    39,
    28,
    40,
    41,
    28,
    27,
    40,
    27,
    38,
    40,
    31,
    28,
    41,
    31,
    30,
    28,
    30,
    26,
    28,
    34,
    31,
    41,
    34,
    33,
    31,
    33,
    29,
    31,
    37,
    34,
    41,
    37,
    36,
    34,
    36,
    32,
    34,
    40,
    37,
    41,
    40,
    39,
    37,
    39,
    35,
    37
  };
  
  glGenBuffers(1, &posVBO);
  glGenBuffers(1, &indicesVBO);

  glBindBuffer(GL_ARRAY_BUFFER, posVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(temp1), temp1, GL_STATIC_DRAW);
    

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(temp2), temp2, GL_STATIC_DRAW);
  
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  
}

