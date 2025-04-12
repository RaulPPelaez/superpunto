#include "Camera.h"
#include <SDL2/SDL.h>

namespace superpunto {
FreeCamera::FreeCamera() {
  this->pos = glm::vec3(-130, -130, -130); // Behind the origin, looking forward
  this->right = glm::vec3(1, 0, 0);        // X = right
  this->up = glm::vec3(0, 0, 1);           // Z = up
  this->front = glm::vec3(0, 1, 0);        // Y = forward
  int mx, my;
  SDL_GetMouseState(&mx, &my);
  zero_mpos = glm::ivec2(mx, my);
  mpos = zero_mpos;

  yaw = 0;
  pitch = 0;
  roll = 0;
  this->mult = 1;
  this->updateCameraVectors();
  cspeed = 0.6;
  mouse_sensitivity = 0.25;
}
void FreeCamera::warp(glm::vec3 np) { this->pos = np; }
glm::vec3 FreeCamera::get_view() { return pos + front; }
glm::mat4 FreeCamera::lookAt() {
  return this->view = glm::lookAt(pos, pos + front, up);
}

#define KEYSTATE(KEY) keystate[SDL_SCANCODE_##KEY]

void FreeCamera::update() {
  const Uint8 *keystate = SDL_GetKeyboardState(NULL);

  if (KEYSTATE(A))
    this->pos += this->right * cspeed * mult;
  if (KEYSTATE(D))
    this->pos -= this->right * cspeed * mult;
  if (KEYSTATE(W))
    this->pos += this->front * cspeed * mult;
  if (KEYSTATE(S))
    this->pos -= this->front * cspeed * mult;
  if (KEYSTATE(LSHIFT))
    this->pos += this->up * cspeed * mult;
  if (KEYSTATE(LCTRL))
    this->pos -= this->up * cspeed * mult;
  if (KEYSTATE(KP_PLUS))
    this->mult *= 1.05;
  if (KEYSTATE(KP_MINUS))
    this->mult *= 0.95;

  if (KEYSTATE(E))
    this->roll = 4 * cspeed;
  if (KEYSTATE(Q))
    this->roll = -4 * cspeed;

  if (KEYSTATE(P)) {
    printf("up: %.3f, %.3f, %.3f  \n", up.x, up.y, up.z);
    printf("right: %.3f, %.3f, %.3f  \n", right.x, right.y, right.z);
    printf("pos: %.3f, %.3f, %.3f  \n", pos.x, pos.y, pos.z);
    printf("front: %.3f, %.3f, %.3f  \n\n", front.x, front.y, front.z);
  }
  if (KEYSTATE(LALT))
    process_mouse();
  this->updateCameraVectors();
}

void FreeCamera::process_mouse() {

  SDL_GetMouseState(&mpos[0], &mpos[1]);
  yaw = (float)((zero_mpos.x - mpos.x)) * mouse_sensitivity;
  pitch = -(float)((zero_mpos.y - mpos.y)) * mouse_sensitivity;
  zero_mpos = mpos;
}

void FreeCamera::updateCameraVectors() {
  glm::quat q1 = glm::angleAxis(glm::radians(pitch), right);
  glm::mat4 rot1 = glm::mat4_cast(q1);
  up = glm::normalize(glm::mat3(rot1) * up);
  glm::quat q2 = glm::angleAxis(glm::radians(yaw), up);
  glm::mat4 rot2 = glm::mat4_cast(q2);
  right = glm::normalize(glm::mat3(rot2) * right);
  front = glm::normalize(glm::cross(right, up));
  glm::quat q3 = glm::angleAxis(glm::radians(roll), front);
  glm::mat4 rot3 = glm::mat4_cast(q3);
  right = glm::normalize(glm::mat3(rot3) * right);
  up = glm::normalize(glm::cross(front, right));
  yaw = 0;
  pitch = 0;
  roll = 0;
}

void FreeCamera::set_origin() {
  int mx, my;
  SDL_GetMouseState(&mx, &my);
  zero_mpos = glm::ivec2(mx, my);
  mpos = zero_mpos;
}

} // namespace superpunto
