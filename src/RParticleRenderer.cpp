#include"RParticleRenderer.h"

namespace superpunto{
  //Vertex per sphere in fill_vbos
#define NVERTEX 240

  RParticleRenderer::RParticleRenderer(std::shared_ptr<System> sys,
				       std::shared_ptr<RWindow> w,
				       std::shared_ptr<Camera> cam, float gscale):
    RRenderer(sys, w, cam, gscale),
    maxN(1),
    fbo(sys, w->getResolution()),
    ssaofbo(sys, w->getResolution()),
    gBuffer(sys, w->getResolution())    
  {

    sys->log<System::DEBUG>("[ParticleRenderer]Initializing OpenGL...");

  
    attribs["in_vertex"] = 0;
    attribs["pos"] = 1;
    attribs["color"] = 2;
    attribs["scale"] = 3;

    init_buffers();
    init_shaders();
    init_uniforms();
    auto resolution = w->getResolution();
    this->handle_resize(resolution.x, resolution.y);

  }

  RParticleRenderer::~RParticleRenderer(){}
                                                                                               

  void RParticleRenderer::handle_resize(uint fw, uint fh){
    RRenderer::handle_resize(fw, fh);
    fbo.handle_resize(fw, fh);
    ssaofbo.handle_resize(fw, fh);
    gBuffer.handle_resize(fw, fh);

    ssaopr.use();
    glUniform1f(glGetUniformLocation(ssaopr.id(), "FWIDTH"), (float)fw);
    glUniform1f(glGetUniformLocation(ssaopr.id(), "FHEIGHT"), (float)fh);  
    ssaopr.unbind();
    CheckGLError("Error at resize");

  }
  bool RParticleRenderer::init_buffers(){
    sys->log<System::DEBUG>("[ParticleRenderer]Init buffers...     ");
    
    DataLayout dl;
    dl.init(3, 3*sizeof(float), GL_FLOAT, 0);
    lines_vbo.init(GL_ARRAY_BUFFER, GL_DYNAMIC_STORAGE_BIT, dl);
    lines_vbo.initmem(2*sizeof(float)*3, NULL);
  
    init_sphere();
    init_instance_vbos();
    init_vao();
    return true;
  }
  bool RParticleRenderer::init_sphere(){ //Config and upload sphere vertices
    DataLayout dl;
    dl.init(3, 3*sizeof(float), GL_FLOAT, 0);
    sphere_vbos[0].init(GL_ARRAY_BUFFER, GL_MAP_READ_BIT, dl);
    sphere_vbos[1].init(GL_ELEMENT_ARRAY_BUFFER, GL_MAP_READ_BIT);
    fill_sphere_vbos(sphere_vbos[0], sphere_vbos[1]);
    CheckGLError("Error in init_sphere");    
    return true;
  }

  bool RParticleRenderer::init_vao(){ //Configure Vertex Array Objects
    spheres_vao.set_attrib(attribs["in_vertex"], sphere_vbos[0], 0);
    spheres_vao.set_attrib(attribs["pos"], instances_vbos[0], 1);  
    spheres_vao.set_attrib(attribs["color"], instances_vbos[1], 2);
    spheres_vao.set_attrib(attribs["scale"], instances_vbos[2], 3);

    line_vao.set_attrib(0, lines_vbo, 0);
    CheckGLError("Error in init_vao");    
    
    return true;
  }

  bool RParticleRenderer::init_instance_vbos(){
    DataLayout dl;
    dl.init(3, 3*sizeof(float), GL_FLOAT, 0, 1);
    instances_vbos[0].init(GL_ARRAY_BUFFER, GL_DYNAMIC_STORAGE_BIT, dl);
    instances_vbos[1].init(GL_ARRAY_BUFFER, GL_DYNAMIC_STORAGE_BIT, dl);
    dl.init(1, sizeof(float), GL_FLOAT, 0, 1);
    instances_vbos[2].init(GL_ARRAY_BUFFER, GL_DYNAMIC_STORAGE_BIT, dl);
    instances_vbos[0].initmem(maxN*sizeof(float)*3, NULL);  //poss
    instances_vbos[1].initmem(maxN*sizeof(float)*3, NULL);  //colors
    instances_vbos[2].initmem(maxN*sizeof(float)*1, NULL);  //scales
    CheckGLError("Error in init_instance_vbos");
    return true;
  }


  bool RParticleRenderer::init_shaders(){
    sys->log<System::DEBUG>("[ParticleRenderer] Init shaders...     ");
    RShader shs[2];
    shs[0].charload(shaders_geom_vs, GL_VERTEX_SHADER);
    shs[1].charload(shaders_geom_fs, GL_FRAGMENT_SHADER);
    //shs[0].load("../src/shaders/geom.vs", GL_VERTEX_SHADER);
    // shs[1].load("../src/shaders/geom.fs", GL_FRAGMENT_SHADER);
    pr.init(shs, 2);

    shs[0].charload(shaders_quad_vs, GL_VERTEX_SHADER);
    shs[1].charload(shaders_light_fs, GL_FRAGMENT_SHADER);
    // shs[0].load("../src/shaders/quad.vs", GL_VERTEX_SHADER);
    // shs[1].load("../src/shaders/light.fs", GL_FRAGMENT_SHADER);

    lightpr.init(shs, 2);

    shs[0].charload(shaders_quad_vs, GL_VERTEX_SHADER);
    shs[1].charload(shaders_ssao_fs, GL_FRAGMENT_SHADER);
    // shs[0].load("../src/shaders/quad.vs", GL_VERTEX_SHADER);
    // shs[1].load("../src/shaders/ssao.fs", GL_FRAGMENT_SHADER);

    ssaopr.init(shs, 2);
  
    shs[0].charload(shaders_line_vs, GL_VERTEX_SHADER);
    shs[1].charload(shaders_line_fs, GL_FRAGMENT_SHADER);
    linepr.init(shs, 2);
  

    ssaofbo.setFormat(GL_R32F, GL_RED, GL_FLOAT);
    CheckGLError("Error in init_shaders");

    return true;
  }
  bool RParticleRenderer::init_uniforms(){
    sys->log<System::DEBUG>("[ParticleRenderer] Init uniforms...    ");
    pr.use();
    uniMVP = glGetUniformLocation(pr.id(), "MVP");
    unimodel = glGetUniformLocation(pr.id(), "model");
    uninormalmodel = glGetUniformLocation(pr.id(), "normal_model");
    pr.setFlag("picking",0);
    pr.setFlag("drawing_picked",0);
    glUniform1f(glGetUniformLocation(pr.id(), "pickscale"),
		1.0f);
    glUniform1f(glGetUniformLocation(pr.id(), "gscale"),
		this->gscale);
    glUniform1f(glGetUniformLocation(pr.id(), "znear"),
		znear);
    glUniform1f(glGetUniformLocation(pr.id(), "zfar"),
	        zfar);
    pr.unbind();

    gBuffer.bindSamplers(lightpr);
    gBuffer.bindSamplers(ssaopr);
  
    lightpr.setFlag("SSAOtex", ssaofbo.getTexUnit());
  
    //ssaofbo.bindColorTex(lightpr);

    auto op = sys->getInputOptions();
    ssaopr.use();
    glUniform1f(glGetUniformLocation(ssaopr.id(), "FWIDTH"), (float)op.target_FW);
    glUniform1f(glGetUniformLocation(ssaopr.id(), "FHEIGHT"), (float)op.target_FH);  
    ssaopr.unbind();
  

    linepr.use();
    glUniform1f(glGetUniformLocation(linepr.id(), "gscale"),
		this->gscale);
    glUniform3f(glGetUniformLocation(linepr.id(), "color"),
		1,1,1);

    linepr.unbind();
    //sys->log<System::DEBUG>("[RParticleRenderer] Checking GL Errors in init_uniforms");
    CheckGLError("Error in init_uniforms");
    return true;
  }


  bool RParticleRenderer::upload_instances(ParticleData pdata){
    int N = pdata.N;
    if(N > maxN){
      sys->log<System::DEBUG>("[RParticleRenderer] Resizing instances to a max of %d", N);
      maxN = N;
      instances_vbos[0].reset();
      instances_vbos[1].reset();
      instances_vbos[2].reset();     
      init_instance_vbos();
      init_vao();
    }
    instances_vbos[0].upload(0, 3*N*sizeof(float),
			     (const void *)pdata.pos);   
    instances_vbos[1].upload(0, 3*N*sizeof(float),
			     (const void *)pdata.colors);
    instances_vbos[2].upload(0, 1*N*sizeof(float),
			     (const void *)pdata.scales);
    this->particles = pdata;
    float3 L = pdata.L;
    
    box.setSize(glm::vec3(L.x, L.y, L.z));
    CheckGLError("Error in upload_instances");
    
    return true;
  }

  void RParticleRenderer::update(){
    RRenderer::update();
  }

  int RParticleRenderer::pick(int x, int y, int pickindex){
    glClearColor(0.0f ,0.0f ,0.0f ,1.0f);
    pr.use();
    pr.setFlag("picking",1);
    this->picking = true;
    pr.unbind();
    //  draw();
    geometry_pass();
    pr.use();
    pr.setFlag("picking",0);
    pr.unbind();
    glm::vec4 pixel = gBuffer.getPixel(x,y);

    this->picked[pickindex] =pixel[0]+256*pixel[1]+ 256*256*pixel[2] - 1;

    //Two colors identify the same index to gain precision,
    //"only" 255^3/2 differenciable objects 
    if(picked[pickindex]>=0) picked[pickindex] /=2;
  
    //cerr<<pixel[0]<<" "<<pixel[1]<<" "<<pixel[2]<<endl<<picked[pickindex]<<endl;
    this->picking = false;
    return picked[pickindex];  
  }
  void RParticleRenderer::render_picked(){
    if(picked[0]<0 && picked[1]<0) return;
  
    glLineWidth(1.3f);
    pr.setFlag("drawing_picked", 1);
    glUniform1f(glGetUniformLocation(pr.id(), "pickscale"), 1.3f);
    if(picked[0]>=0)
      glDrawElementsInstancedBaseInstance(GL_LINE_STRIP, NVERTEX,
					  GL_UNSIGNED_INT,
					  NULL, 1, picked[0]);
    if(picked[1]>=0)
      glDrawElementsInstancedBaseInstance(GL_LINE_STRIP, NVERTEX,
					  GL_UNSIGNED_INT,
					  NULL, 1, picked[1]);
    if(picked[1]>=0 && picked[0]>=0){
      linepr.use();
      
      float lineptr[6];
      fori(0,3) forj(0,2) lineptr[i+3*j] = particles.pos[3*picked[j]+i];

    
      lines_vbo.upload(0, 6*sizeof(float), (const void *)&lineptr[0]);
      glUniformMatrix4fv(glGetUniformLocation(linepr.id(), "MVP")
			 , 1, GL_FALSE, glm::value_ptr(MVP));
      glUniformMatrix4fv(glGetUniformLocation(linepr.id(), "model")
			 , 1, GL_FALSE, glm::value_ptr(model));
      line_vao.use();
      glLineWidth(3.3f);
      glDrawArrays(GL_LINES, 0, 2);
      glLineWidth(1.0f);
      pr.use();
      spheres_vao.use();
    }
    glUniform1f(glGetUniformLocation(pr.id(), "pickscale"), 1.0f);
    pr.setFlag("drawing_picked", 0);
  }

  void RParticleRenderer::geometry_pass(){
    auto cfg = sys->getInputOptions();
    
    gBuffer.use();
    glEnable(GL_DEPTH_TEST);

    glClearColor(cfg.bcolor[0], cfg.bcolor[1], cfg.bcolor[2], 1.0f);  
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    pr.use();
    glUniformMatrix4fv(uniMVP , 1, GL_FALSE, glm::value_ptr(MVP));
    glUniformMatrix4fv(unimodel , 1, GL_FALSE, glm::value_ptr(model));
    glm::mat4 normal_model = transpose(inverse(model));
    glUniformMatrix4fv(uninormalmodel , 1, GL_FALSE, glm::value_ptr(normal_model));


    spheres_vao.use();
    sphere_vbos[1].use(); //indices

    glDrawElementsInstanced(GL_TRIANGLES, NVERTEX, GL_UNSIGNED_INT, NULL, particles.N);
    if(!picking) render_picked();
  
    if(!cfg.nobox) box.draw();
  
    sphere_vbos[1].unbind(); //indices
    spheres_vao.unbind(); 
    pr.unbind();

    gBuffer.unbind();

  }


  void RParticleRenderer::light_pass(){
    fbo.use();
    glDisable(GL_DEPTH_TEST);
    lightpr.use();
    glUniform3f(glGetUniformLocation(lightpr.id(), "viewPos"), cam->pos.x, cam->pos.y, cam->pos.z);

    dummy_vao.use();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    dummy_vao.unbind();

    lightpr.unbind();
    fbo.unbind();
  }


  GLfloat lerp(GLfloat a, GLfloat b, GLfloat f){
    return a + f * (b - a);
  }
  void RParticleRenderer::SSAO_pass(){
    static const int nsamples = 129;
    static glm::vec4 points[nsamples];
    static bool gen_points = true;
  
    if(gen_points){
      fori(0,nsamples){
	/*We will sample depths in a semisphere*/
	auto randEsp = [](){return rand()/(float)RAND_MAX;};
	glm::vec4 sample =
	  glm::normalize(
			 glm::vec4(randEsp()*2.0-1.0,
				   randEsp()*2.0-1.0,
				   randEsp(),0)
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

  void RParticleRenderer::SSAOrad(float inc){
    static float rad = 0.4f;
    rad += inc;
    glProgramUniform1f(ssaopr.id(),
		       glGetUniformLocation(ssaopr.id(),"radius"),
		       rad);    
  }
  void RParticleRenderer::draw(){
    //glViewport(0,0,FWIDTH, FHEIGHT);
    //pr.setFlag("picking",1); //Uncomment for picking view
  
    geometry_pass();
    SSAO_pass();
    light_pass();

    glDisable(GL_DEPTH_TEST);  
    fbo.draw();
    RRenderer::display();
  }




  Uint8* RParticleRenderer::getPixels(){
  
    return fbo.getColorData();
  }

  glm::int2 RParticleRenderer::getSize(){
    return fbo.getSize();
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
}
