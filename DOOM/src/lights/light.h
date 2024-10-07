#include <glm/glm.hpp>
#include <string>

#pragma once

class Light {
  glm::vec3 color = glm::vec3(1.0f);
  glm::vec3 position = glm::vec3(0);
  glm::vec3 diffuse = glm::vec3(1.0f);
  glm::vec3 specular = glm::vec3(1.0f);
  float intensity = 1.0f;

public:
  glm::vec3 getPosition() { return position; }
  void setPosition(glm::vec3 position_) { position = position_; }
  float getIntensity() { return intensity; }
  void setIntensity(float intensity_) { intensity = intensity_; }
  glm::vec3 getColor() { return color; }
  void setColor(glm::vec3 color_) { color = color_; }
  glm::vec3 getDiffuse() { return diffuse; }
  void setDiffuse(glm::vec3 diffuse_) { diffuse = diffuse_; }
  glm::vec3 getSpecular() { return specular; }
  void setSpecular(glm::vec3 specular_) { specular = specular_; }

  virtual std::string getType() { return "Light"; }
};
