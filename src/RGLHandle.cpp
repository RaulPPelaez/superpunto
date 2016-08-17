#include "RGLHandle.h"


#define FOV glm::radians(45.0f)
#define ZNEAR 0.01f
#define ZFAR 1000.0f

RGLHandle::RGLHandle(int maxN, float gscale, RConfig cfg):
  picked(-1),
  maxN(maxN),
  gscale(gscale),
  cfg(cfg){

  printf("Initializing OpenGL...\n");
  this->handle_resize();

  
  attribs["in_vertex"] = 0;
  attribs["pos"] = 1;
  attribs["color"] = 2;
  attribs["scale"] = 3;

  init_buffers();
  init_math();
  init_shaders();
  init_uniforms();
  printf("DONE!\n");
}

RGLHandle::~RGLHandle(){}
                                                                                               


bool RGLHandle::init_buffers(){
  printf("\tInit buffers...     ");
  init_sphere();
  init_instance_vbos();
  init_vao();

  printf("DONE!\n");
  return true;
}
bool RGLHandle::init_sphere(){ //Config and upload sphere vertices
  DataLayout dl;
  dl.init(3, 3*sizeof(float), GL_FLOAT, 0);
  sphere_vbos[0].init(GL_ARRAY_BUFFER, GL_MAP_READ_BIT, dl);
  sphere_vbos[1].init(GL_ELEMENT_ARRAY_BUFFER, GL_MAP_READ_BIT);
  fill_sphere_vbos(sphere_vbos[0], sphere_vbos[1]);
  return true;
}

bool RGLHandle::init_vao(){ //Configure Vertex Array Object
  spheres_vao.set_attrib(attribs["in_vertex"], sphere_vbos[0], 0);
  spheres_vao.set_attrib(attribs["pos"], instances_vbos[0], 1);  
  spheres_vao.set_attrib(attribs["color"], instances_vbos[1], 2);
  spheres_vao.set_attrib(attribs["scale"], instances_vbos[2], 3);
  return true;
}

bool RGLHandle::init_instance_vbos(){
  DataLayout dl;
  dl.init(3, 3*sizeof(float), GL_FLOAT, 0, 1);
  instances_vbos[0].init(GL_ARRAY_BUFFER, GL_DYNAMIC_STORAGE_BIT, dl);
  instances_vbos[1].init(GL_ARRAY_BUFFER, GL_DYNAMIC_STORAGE_BIT, dl);
  dl.init(1, sizeof(float), GL_FLOAT, 0, 1);
  instances_vbos[2].init(GL_ARRAY_BUFFER, GL_DYNAMIC_STORAGE_BIT, dl);
  //Create buffer with max possible N
  instances_vbos[0].initmem(maxN*sizeof(float)*3, NULL);  //poss
  instances_vbos[1].initmem(maxN*sizeof(float)*3, NULL);  //colors
  instances_vbos[2].initmem(maxN*sizeof(float)*1, NULL);  //scales
  return true;
}


bool RGLHandle::init_math(){
  proj =glm::perspective(FOV, FWIDTH/(float)FHEIGHT, ZNEAR, ZFAR);
  model = glm::mat4();
  view = cam.lookAt();
  rotate_model(M_PI/4.0f, 1.0f, 0.0f, 0.0f);

  MVP = proj*view*model;

  return true;
}


extern const char* VS_SOURCE;
extern const char* FS_SOURCE;
bool RGLHandle::init_shaders(){
  printf("\tInit shaders...     ");
  RShader shs[2];
  // shs[0].charload(VS_SOURCE, GL_VERTEX_SHADER);
  // shs[1].charload(FS_SOURCE, GL_FRAGMENT_SHADER);
  shs[0].load("../src/shaders/geom.vs", GL_VERTEX_SHADER);
  shs[1].load("../src/shaders/geom.fs", GL_FRAGMENT_SHADER);
  pr.init(shs, 2);

  shs[0].load("../src/shaders/quad.vs", GL_VERTEX_SHADER);
  shs[1].load("../src/shaders/light.fs", GL_FRAGMENT_SHADER);
  lightpr.init(shs, 2);

  shs[0].load("../src/shaders/quad.vs", GL_VERTEX_SHADER);
  shs[1].load("../src/shaders/ssao.fs", GL_FRAGMENT_SHADER);
  ssaopr.init(shs, 2);

  ssaofbo.setFormat(GL_R32F, GL_RED, GL_FLOAT);
  //CheckGLError();

  printf("DONE!\n");
  return true;
}
bool RGLHandle::init_uniforms(){
  printf("\tInit uniforms...    ");
  pr.use();
  uniMVP = glGetUniformLocation(pr.id(), "MVP");
  unimodel = glGetUniformLocation(pr.id(), "model");
  pr.setFlag("picking",0);
  pr.setFlag("drawing_picked",0);
  glUniform1f(glGetUniformLocation(pr.id(), "pickscale"),
	      1.0f);
  glUniform1f(glGetUniformLocation(pr.id(), "gscale"),
	      this->gscale);
  glUniform1f(glGetUniformLocation(pr.id(), "znear"),
	      ZNEAR);
  glUniform1f(glGetUniformLocation(pr.id(), "zfar"),
	      ZFAR);
  pr.unbind();

  gBuffer.bindSamplers(lightpr);
  gBuffer.bindSamplers(ssaopr);
  
  lightpr.setFlag("SSAOtex", ssaofbo.getTexUnit());
  
  //ssaofbo.bindColorTex(lightpr);




  printf("DONE!\n");
  return true;
}


bool RGLHandle::upload_instances(const float *pos, const float *colors, const float *scales, int N){
  instances_vbos[0].upload(0, 3*N*sizeof(float), (const void *)pos);
  instances_vbos[1].upload(0, 3*N*sizeof(float), (const void *)colors);
  instances_vbos[2].upload(0, 1*N*sizeof(float), (const void *)scales);
  currentN = N;
  return true;
}

void RGLHandle::handle_resize(){
  glViewport(0,0,FWIDTH, FHEIGHT);
  float ar = FWIDTH/(float)FHEIGHT;
  proj =glm::perspective(FOV, ar, ZNEAR, ZFAR);
  //gBuffer.handle_resize();
  //fbo.handle_resize();
  //ssaofbo.handle_resize();
}

void RGLHandle::rotate_model(GLfloat angle, GLfloat x, GLfloat y, GLfloat z){
  model = glm::rotate(model, angle,  glm::vec3(x, y, z));
}


Uint8* RGLHandle::getPixels(){
  return fbo.getColorData();
}


void RGLHandle::update(){
  cam.update();
  
}
int RGLHandle::pick(int x, int y){
  glClearColor(0.0f ,0.0f ,0.0f ,1.0f);
  pr.use();
  pr.setFlag("picking",1);
  pr.unbind();
  //  draw();
  geometry_pass();
  pr.use();
  pr.setFlag("picking",0);
  pr.unbind();
  glm::vec4 pixel = gBuffer.getPixel(x,y);
  picked =pixel[0]+256*pixel[1]+ 256*256*pixel[2] - 1;
  //Two colors identify the same index to gain precision,
  //"only" 255^3/2 differenciable objects 
  if(picked>=0) picked /=2;
  //  cerr<<pixel[0]<<" "<<pixel[1]<<" "<<pixel[2]<<endl<<picked<<endl;
  return picked;
}
void RGLHandle::render_picked(){
  if(picked>=0){
    glLineWidth(1.3f);
    pr.setFlag("drawing_picked", 1);
    glUniform1f(glGetUniformLocation(pr.id(), "pickscale"), 1.3f);
    glDrawElementsInstancedBaseInstance(GL_LINE_STRIP, 240,
					GL_UNSIGNED_INT, NULL, 1, picked);
    glUniform1f(glGetUniformLocation(pr.id(), "pickscale"), 1.0f);
    pr.setFlag("drawing_picked", 0);
  }
}
void RGLHandle::geometry_pass(){

  gBuffer.use();
  glEnable(GL_DEPTH_TEST);
  glClearColor(cfg.bcolor[0], cfg.bcolor[1], cfg.bcolor[2], 1.0f);  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  view = cam.lookAt();
  MVP = proj*view*model;

  pr.use();
  glUniformMatrix4fv(uniMVP , 1, GL_FALSE, glm::value_ptr(MVP));
  glUniformMatrix4fv(unimodel , 1, GL_FALSE, glm::value_ptr(model));


  spheres_vao.use();
  sphere_vbos[1].use(); //indices
  glDrawElementsInstanced(GL_TRIANGLES, 240, GL_UNSIGNED_INT, NULL, currentN);
  render_picked();
  sphere_vbos[1].unbind(); //indices
  spheres_vao.unbind(); 
  pr.unbind();

  gBuffer.unbind();
}


void RGLHandle::light_pass(){
  fbo.use();
  glDisable(GL_DEPTH_TEST);
  lightpr.use();
  glUniform3f(glGetUniformLocation(lightpr.id(), "viewPos"), cam.pos.x, cam.pos.y, cam.pos.z);

  dummy_vao.use();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  dummy_vao.unbind();

  lightpr.unbind();
  fbo.unbind();
}


GLfloat lerp(GLfloat a, GLfloat b, GLfloat f){
  return a + f * (b - a);
}
void RGLHandle::SSAO_pass(){
  static const int nsamples = 129;
  static glm::vec4 points[nsamples];
  static bool gen_points = true;
  
  if(gen_points){
    fori(0,nsamples){
      /*We will sample depths in a semisphere*/
      glm::vec4 sample =
	glm::normalize(
		       glm::vec4(RANDESP*2.0-1.0,
				 RANDESP*2.0-1.0,
				 RANDESP,0)
		       );
      float scale = float(i)/nsamples;
      /*We want the samples to be preferently close*/
      scale = lerp(0.1f, 1.0f, scale*scale);
      sample *= scale;
      points[i] = sample;
    }
    /*Upload the sample points*/
    glProgramUniform4fv(ssaopr.id(),
			glGetUniformLocation(ssaopr.id(),
					     "points"),
			nsamples, glm::value_ptr(points[0]));
    gen_points = false;
  }

  ssaofbo.use();
  // glUniformMatrix4fv(glGetUniformLocation(ssaopr.id(), "proj"),
  // 		     1, GL_FALSE, glm::value_ptr(proj));

  glDisable(GL_DEPTH_TEST);
  ssaopr.use();

  dummy_vao.use();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  dummy_vao.unbind();

  ssaopr.unbind();
  ssaofbo.unbind();
}

void RGLHandle::SSAOrad(float inc){
  static float rad = 0.4f;
  rad += inc;
  glProgramUniform1f(ssaopr.id(),
		     glGetUniformLocation(ssaopr.id(),"radius"),
		     rad);
  cerr<<rad<<endl;
}
void RGLHandle::draw(){
  //glViewport(0,0,FWIDTH, FHEIGHT);
  //pr.setFlag("picking",1); //Uncomment for picking view

  geometry_pass();
  SSAO_pass();
  light_pass();

  glDisable(GL_DEPTH_TEST);

  fbo.draw();
}


void fill_sphere_vbos(VBO &posVBO, VBO &indicesVBO){

  float v[]={
    0.0000, -1.0000,  0.0000,  0.4253, -0.8507,  0.3090,
    -0.1625, -0.8507,  0.5000,  0.7236, -0.4472,  0.5257,
    0.8506, -0.5257,  0.0000, -0.5257, -0.8507,  0.0000,
    -0.1625, -0.8507, -0.5000,  0.4253, -0.8507, -0.3090,
    0.9511,  0.0000,  0.3090, -0.2764, -0.4472,  0.8506,
    0.2629, -0.5257,  0.8090,  0.0000,  0.0000,  1.0000,
    -0.8944, -0.4472,  0.0000, -0.6882, -0.5257,  0.5000,
    -0.9511,  0.0000,  0.3090, -0.2764, -0.4472, -0.8506,
    -0.6882, -0.5257, -0.5000, -0.5878,  0.0000, -0.8090,
    0.7236, -0.4472, -0.5257,  0.2629, -0.5257, -0.8090,
    0.5878,  0.0000, -0.8090,  0.5878,  0.0000,  0.8090,
    -0.5878,  0.0000,  0.8090, -0.9511,  0.0000, -0.3090,
    0.0000,  0.0000, -1.0000,  0.9511,  0.0000, -0.3090,
    0.2764,  0.4472,  0.8506,  0.6882,  0.5257,  0.5000,
    0.1625,  0.8507,  0.5000, -0.7236,  0.4472,  0.5257,
    -0.2629,  0.5257,  0.8090, -0.4253,  0.8507,  0.3090,
    -0.7236,  0.4472, -0.5257, -0.8506,  0.5257,  0.0000,
    -0.4253,  0.8507, -0.3090,  0.2764,  0.4472, -0.8506,
    -0.2629,  0.5257, -0.8090,  0.1625,  0.8507, -0.5000,
    0.8944,  0.4472,  0.0000,  0.6882,  0.5257, -0.5000,
    0.5257,  0.8507,  0.0000,  0.0000,  1.0000,  0.0000
  };
  GLuint index[]={
    0,  1,  2,  3,  1,  4,  0,  2,  5,  0, 
    5,  6,  0,  6,  7,  3,  4,  8,  9, 10, 
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 
    3,  8, 21,  9, 11, 22, 12, 14, 23, 15, 
    17, 24, 18, 20, 25, 26, 27, 28, 29, 30, 
    31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 
    2, 10,  9,  2,  1, 10,  1,  3, 10,  4, 
    7, 18,  4,  1,  7,  1,  0,  7,  5, 13, 
    12,  5,  2, 13,  2,  9, 13,  6, 16, 15, 
    6,  5, 16,  5, 12, 16,  7, 19, 18,  7, 
    6, 19,  6, 15, 19,  8, 25, 38,  8,  4, 
    25,  4, 18, 25, 11, 21, 26, 11, 10, 21, 
    10,  3, 21, 14, 22, 29, 14, 13, 22, 13, 
    9, 22, 17, 23, 32, 17, 16, 23, 16, 12, 
    23, 20, 24, 35, 20, 19, 24, 19, 15, 24, 
    21, 27, 26, 21,  8, 27,  8, 38, 27, 22, 
    30, 29, 22, 11, 30, 11, 26, 30, 23, 33, 
    32, 23, 14, 33, 14, 29, 33, 24, 36, 35, 
    24, 17, 36, 17, 32, 36, 25, 39, 38, 25, 
    20, 39, 20, 35, 39, 28, 40, 41, 28, 27, 
    40, 27, 38, 40, 31, 28, 41, 31, 30, 28, 
    30, 26, 28, 34, 31, 41, 34, 33, 31, 33, 
    29, 31, 37, 34, 41, 37, 36, 34, 36, 32, 
    34, 40, 37, 41, 40, 39, 37, 39, 35, 37
  };
  posVBO.initmem(sizeof(v), v);
  indicesVBO.initmem(sizeof(index), index);
}

