#include <glm/glm.hpp>

#pragma once

class Light {
  glm::vec3 color = glm::vec3(1.0f);
  glm::vec3 position = glm::vec3(0);
  float intensity = 1.0f;

public:
  glm::vec3 getPosition() { return position; }
  void setPosition(glm::vec3 position_) { position = position_; }
  float getIntensity() { return intensity; }
  void setIntensity(float intensity_) { intensity = intensity_; }
  glm::vec3 getColor() { return color; }
  void setColor(glm::vec3 color_) { color = color_; }
};
