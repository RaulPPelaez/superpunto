#include "Camera.h"
#include "System.h"
#include <SDL2/SDL.h>

namespace superpunto {

FreeCamera::FreeCamera() {
  int mx, my;
  SDL_GetMouseState(&mx, &my);
  zero_mpos = glm::ivec2(mx, my);
  this->mult = 1;
  mpos = zero_mpos;
  cspeed = 0.6;
  mouse_sensitivity = 0.25;
  this->reset_camera_view();
}

void FreeCamera::reset_camera_view() {
  this->pos = glm::vec3(-130, -130, -130); // Behind the origin, looking forward
  this->right = glm::vec3(1, 0, 0);        // X = right
  this->up = glm::vec3(0, 0, 1);           // Z = up
  this->front = glm::vec3(0, 1, 0);        // Y = forward
  this->yaw = 0;
  this->pitch = 0;
  this->roll = 0;
  this->updateCameraVectors();
}
void FreeCamera::warp(glm::vec3 np) { this->pos = np; }

glm::mat4 FreeCamera::lookAt() {
  // Recompute right and up based on the default world up
  return this->view = glm::lookAt(pos, pos + front, up);
}

glm::mat4 FreeCamera::lookAt(glm::vec3 target) {
  // Compute the new front direction toward the target
  this->front = glm::normalize(target - this->pos);
  // Use a fixed "world up" vector (Z-up) to compute a new basis
  glm::vec3 worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
  // Recompute right and up so that front faces the target
  this->right = glm::normalize(glm::cross(worldUp, this->front));
  this->up = glm::normalize(glm::cross(this->front, this->right));
  yaw = pitch = roll = 0;
  return this->lookAt();
}

#define KEYSTATE(KEY) keystate[SDL_SCANCODE_##KEY]

void FreeCamera::handle_event(SDL_Event &e) {
  if (e.type == SDL_TEXTINPUT) {
    const char *txt = e.text.text;
    if (txt[0] == '+' && txt[1] == '\0') {
      this->mult *= 1.05f;
    } else if (txt[0] == '-' && txt[1] == '\0') {
      this->mult *= 0.95f;
    }
  }
}

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
  if (KEYSTATE(E))
    this->roll = 4 * cspeed;
  if (KEYSTATE(Q))
    this->roll = -4 * cspeed;
  if (KEYSTATE(P)) {
    System::log<System::MESSAGE>("up: %.3f, %.3f, %.3f", up.x, up.y, up.z);
    System::log<System::MESSAGE>("right: %.3f, %.3f, %.3f", right.x, right.y,
                                 right.z);
    System::log<System::MESSAGE>("pos: %.3f, %.3f, %.3f", pos.x, pos.y, pos.z);
    System::log<System::MESSAGE>("front: %.3f, %.3f, %.3f", front.x, front.y,
                                 front.z);
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
