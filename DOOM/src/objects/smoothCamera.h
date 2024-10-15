#include "camera.h"
#include <glm/glm.hpp>

class SmoothCamera : public Camera {
  glm::vec3 velocity;
  float speed = 0.3f;
  float drag = 0.9f;

public:
  *SmoothCamera(float width, float height) : Camera(width, height) {}
  void addVelocity(glm::vec3 velocity_) { velocity += velocity_; }
  void calc() {
    setPosition(getPosition() + velocity * speed);
    velocity *= drag;
  }
};
