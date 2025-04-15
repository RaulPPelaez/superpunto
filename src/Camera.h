#ifndef CAMERA_H
#define CAMERA_H

#include "defines.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace superpunto {
class FreeCamera {
public:
  FreeCamera();

  void warp(glm::vec3 np);
  glm::mat4 lookAt();
  glm::mat4 lookAt(glm::vec3 target);

  void reset_camera_view();

  void update();
  void process_mouse();
  void updateCameraVectors();
  void set_origin();

  glm::ivec2 zero_mpos;
  glm::ivec2 mpos;

  glm::mat4 view;
  glm::vec3 pos, up, front, right;
  float yaw, pitch, roll;

  float mult;
  float cspeed;
  float mouse_sensitivity;
};
} // namespace superpunto
#endif
