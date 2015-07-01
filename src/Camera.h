#ifndef CAMERA_H
#define CAMERA_H

#include "defines.h"

#include <GL/gl.h>
#include <SFML/Window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>



class Camera{
  public:
   Camera(){
	  this-> pos = glm::vec3(0, 10, -10                                                                         ); 
	  this->right = glm::vec3(-1,0,0);
	  this->up = glm::vec3(0,-1/sqrt(2),-1/sqrt(2));
	  this->front =(glm::cross(right,up));
	  
	  zero_mpos = sf::Vector2i(FWIDTH/2, FHEIGHT/2);
	  yaw = 0;
	  pitch = 0;
	  this->mult = 1;
	}

    glm::vec3 get_view(){return pos+front;}
	glm::mat4 lookAt(){return this->view = glm::lookAt(pos, pos+front, up);}

    void update();
    virtual void process_mouse() = 0;

    virtual void updateCameraVectors() = 0;

    sf::Vector2i zero_mpos;
    sf::Vector2i mpos;
    
    glm::mat4 view;
    glm::vec3 pos, up, front, right;
    GLfloat yaw, pitch, roll;
    
    float mult;
};

class FreeCamera: public Camera{
  public:
    FreeCamera(){this->mult = 1;}

    virtual void process_mouse();
    virtual void updateCameraVectors();

 };
 
 class FPSCamera: public Camera{
  public:
    FPSCamera(){
		this->pos   = glm::vec3(0.0f, 10.0f,  -1.0f);
		this->up = glm::vec3(0.0f, 1.0f,  0.0f);
	}

    virtual void process_mouse();
    virtual void updateCameraVectors();

 };




#endif