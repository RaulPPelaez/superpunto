#include "Camera.h"

	

void FreeCamera::update(){
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) this->pos += this->right*CAMSPEED*mult;
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) this->pos -= this->right*CAMSPEED*mult;
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) this->pos += this->front*CAMSPEED*mult;
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))this->pos -= this->front*CAMSPEED*mult;
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) this->pos += this->up*CAMSPEED*mult;
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))this->pos -= this->up*CAMSPEED*mult;
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Add)) this->mult*=1.01;
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract)) this->mult *=0.99;

  if(sf::Keyboard::isKeyPressed(sf::Keyboard::E))this->roll = 4*CAMSPEED;
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q))this->roll = -4*CAMSPEED;

  if(sf::Keyboard::isKeyPressed(sf::Keyboard::P)){
	  printf("up: %.3f, %.3f, %.3f  \n", up.x, up.y, up.z);
	  printf("right: %.3f, %.3f, %.3f  \n", right.x, right.y, right.z);
	  printf("pos: %.3f, %.3f, %.3f  \n", pos.x, pos.y, pos.z);
	  printf("front: %.3f, %.3f, %.3f  \n\n", front.x, front.y, front.z);
  }
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) process_mouse();
  else this->updateCameraVectors();
}

void FreeCamera::process_mouse(){
  
  mpos = sf::Mouse::getPosition();
  //if( (mpos.x == zero_mpos.x) && (mpos.y == zero_mpos.y) ) return;
  sf::Mouse::setPosition(zero_mpos);				

  yaw  = (float)( (zero_mpos.x - mpos.x) ) * MOUSE_SENS;		
  pitch = -(float)( (zero_mpos.y - mpos.y) ) * MOUSE_SENS;

  this->updateCameraVectors();	
}
void FreeCamera::updateCameraVectors(){
  
  
   
  glm::quat q1 = glm::angleAxis(glm::radians(pitch), right);
  glm::mat4 rot1 = glm::mat4_cast(q1);
  up =  glm::normalize(glm::mat3(rot1)*up);  
  
  
  glm::quat q2 = glm::angleAxis(glm::radians(yaw), up);
  glm::mat4 rot2 = glm::mat4_cast(q2);
  right = glm::normalize((glm::mat3(rot2*rot1)*right));
	
 
  
  front = glm::normalize(glm::cross(right,up));
  glm::quat q3 = glm::angleAxis(glm::radians(roll), front);
  glm::mat4 rot3 = glm::mat4_cast(q3);
  
  right =  glm::normalize(glm::mat3(rot3)*right);
  up =  glm::normalize(glm::cross(front,right));

  
  roll = 0;

  
}
	
	
	
/*
void FPSCamera::process_mouse(){
  mpos = sf::Mouse::getPosition();
  //if( (mpos.x == zero_mpos.x) && (mpos.y == zero_mpos.y) ) return;
  sf::Mouse::setPosition(zero_mpos);				

    GLfloat xoffset = (float)( (zero_mpos.x - mpos.x) );
    GLfloat yoffset = -(float)( (zero_mpos.y - mpos.y) ); 

    yaw   += xoffset*MOUSE_SENS;
    pitch += yoffset*MOUSE_SENS;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;
	
    this->updateCameraVectors();
}
void FPSCamera::updateCameraVectors(){
  
 front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

  
}
	
*/
	