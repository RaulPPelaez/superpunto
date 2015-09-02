#include "RGL.h"


void RGL::initialize(){
  
  frames = 0.0;

  drawables.initialize(&MVP, &model);

  MVP = glm::mat4();
  model = glm::mat4();

  initBuffers();


  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  proj = glm::perspective(45.0f, 1.0f, 0.01f, 10000.0f);

  lightpos = glm::vec3(0,0,-10);
  loadIdentity();
  printf("DONE!\nDrawing...\n");
  drawables.config_light();
  
  init_post_processing();
}

void RGL::init_post_processing(){


  
  RShader vs, fs;
  const char* VS_SOURCE = GLSL(130,                                                        
			       in vec2 in_vertex;
			       in vec2 texCoords;
			       out vec2 TexCoords;
			       void main() {                                               
				 gl_Position = vec4(in_vertex.xy, 0.0, 1.0);
				 TexCoords = texCoords;
			       }        
			       );
  
  vs.charload(VS_SOURCE, GL_VERTEX_SHADER);
  fs.load("../res/sc.fs", GL_FRAGMENT_SHADER);

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

}

void RGL::setUniform(const char* name, float val){
 drawables.get_pr()->use();

  glUniform1f(glGetUniformLocation(drawables.get_pr()->id(), name), val);

  drawables.get_pr()->unbind();
}
void RGL::draw_to_fb(){
  glBindFramebuffer(GL_FRAMEBUFFER, fb);


}

void RGL::draw(){
  glEnable(GL_DEPTH_TEST);  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  this->view = cam.lookAt();
  drawables.get_pr()->use();

  glUniform3f(glGetUniformLocation(drawables.get_pr()->id(), "EyeWorldPos"), cam.pos.x, cam.pos.y, cam.pos.z);
  glUniform3f(glGetUniformLocation(drawables.get_pr()->id(),"point_light.Position"), lightpos.x, lightpos.y, lightpos.z);

  upload_MVP();
  drawables.draw_model();

}

void RGL::post_process(){
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);

  pp_pr.use();
  glBindVertexArray(pp_vao);
  glBindTexture(GL_TEXTURE_2D, pp_tex);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
  pp_pr.unbind();
}


void RGL::update(){this->cam.update();}

void RGL::handle_event(sf::Event event){
  if (event.type == Event::KeyPressed){
    IF_KEY(Up, lightpos.z += 0.5;)
      IF_KEY(Down, lightpos.z -= 0.5;)
      IF_KEY(Space, upload_step();)

      //IF_KEY(R, current_step= Rmax(current_step-2,0);upload_step();)

      //      IF_KEY(M, play_movie = !play_movie;)

      IF_KEY(Num4, rotate(0.1f,1,0,0);)
      IF_KEY(Num5, rotate(0.1f,0,1,0);)
      IF_KEY(Num6, rotate(0.1f,0,0,1);)

      IF_KEY(Num1, rotate(-0.1f,1,0,0);)
      IF_KEY(Num2, rotate(-0.1f,0,1,0);)
      IF_KEY(Num3, rotate(-0.1f,0,0,1);)
      }

}


void RGL::rotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z){
  model = glm::rotate(model, angle,  glm::vec3(x, y, z));
}
void RGL::scale(GLfloat x, GLfloat y, GLfloat z){
  model = glm::scale(model, glm::vec3(x, y, z));
}
void RGL::translate(GLfloat x, GLfloat y, GLfloat z){
  model = glm::translate(model, glm::vec3(x, y, z));
}
void RGL::lookAt(const glm::vec3 &pos, const glm::vec3 &view, const glm::vec3 &up){
  this->view = glm::lookAt(pos,view,up);
}

void RGL::loadIdentity(){model = glm::mat4();}

void RGL::upload_MVP(){MVP = proj*view*model;}



