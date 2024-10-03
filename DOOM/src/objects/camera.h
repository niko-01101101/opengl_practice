#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma once
class Camera {
  glm::mat4 perspective =
      glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 rotation = glm::vec3(0.01f, 0.01f, 0.01f);
  glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

public:
  glm::mat4 LookAt(glm::vec3 point) { return glm::lookAt(position, point, up); }
  glm::mat4 getPerspective() { return perspective; }
  void setPerspective(glm::mat4 perspective_) { perspective = perspective_; }
  glm::vec3 getPosition() { return position; }
  void setPosition(glm::vec3 position_) { position = position_; }
  glm::vec3 getRotation() { return rotation; }
  void setRotation(glm::vec3 rotation_) { rotation = rotation_; }

  glm::vec3 getForward() { return forward; }
  void setForward(glm::vec3 forward_) { forward = forward_; }
  glm::vec3 getUp() { return up; }
  glm::vec3 getRight() { return glm::normalize(glm::cross(forward, up)); }
};
