#include"RPostProcessor.h"

void RPostProcessor::init(){
  RShader vs, fs;
  const char* VS_SOURCE = GLSL(330,                                                        
			       in vec2 in_vertex;
			       in vec2 texCoords;
			       out vec2 TexCoords;
			       void main() {                                               
				 gl_Position = vec4(in_vertex.xy, 0.0, 1.0);
				 TexCoords = texCoords;
			       }        
			       );
  
  const char* SC_SOURCE = GLSL(330,
			       in vec2 TexCoords;
			       out vec4 color;
			       uniform sampler2D scTex;
			       const float offset = 1.0/200.0;
			       const vec2 offsets[9] = vec2[](
				      vec2(-offset, offset),  
				      vec2(0.0f,    offset),  
				      vec2(offset,  offset),  
				      vec2(-offset, 0.0f),    
				      vec2(0.0f,    0.0f), 
				      vec2(offset,  0.0f),    
				      vec2(-offset, -offset), 
				      vec2(0.0f,    -offset), 
				      vec2(offset,  -offset) 
							      );
			       const float kernel[9] = float[](
							       1, 2, -1,
							       0, 0, 0,
							       4, -3, 0
							       );
			       const float kernel_mod = 3.0;
			       void main() {                                               
  				 color = texture(scTex, TexCoords);
				 return;
  				 vec3 sampleTex[9];
				 for(int i = 0; i < 9; i++){
				     sampleTex[i] = vec3(texture(scTex, TexCoords.st + offsets[i]));
				   }
				 vec3 col;
				 for(int i = 0; i < 9; i++)
				   col += sampleTex[i] * kernel[i]/kernel_mod;
  				 color = vec4(col,1.0);
			       }        
			       );
  vs.charload(VS_SOURCE, GL_VERTEX_SHADER);
  fs.load(SC_SOURCE, GL_FRAGMENT_SHADER);

  pp_pr.create();
  pp_pr.add_shader(vs);
  pp_pr.add_shader(fs);
  pp_pr.link();
  pp_pr.use(); 

  GLfloat quadVertices[] = { 
    // Positions   // TexCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
  };

  glGenVertexArrays(1, &pp_vao);
  glGenBuffers(1, &pp_vbo);
  glBindVertexArray(pp_vao);
  glBindBuffer(GL_ARRAY_BUFFER, pp_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  pp_pr.set_attrib("in_vertex", 2, 4*sizeof(GLfloat), 0);
  pp_pr.set_attrib("texCoords", 2, 4*sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)) );
  glBindVertexArray(0);

  glGenFramebuffers(1, &fb);
  glBindFramebuffer(GL_FRAMEBUFFER, fb);
  
  glGenTextures(1, &pp_tex);
  glBindTexture(GL_TEXTURE_2D, pp_tex); 
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FWIDTH, FHEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pp_tex, 0);


  glGenRenderbuffers(1, &rb);
  glBindRenderbuffer(GL_RENDERBUFFER, rb); 
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, FWIDTH, FHEIGHT);  
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rb);
  
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);  
  
  this->enabled = true;
}


void RPostProcessor::post_process(){
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  
  pp_pr.use();
  glBindVertexArray(pp_vao);
  glBindTexture(GL_TEXTURE_2D, pp_tex);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
  pp_pr.unbind();
   glEnable(GL_DEPTH_TEST);
}
