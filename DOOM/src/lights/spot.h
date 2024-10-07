#include "light.h"
#include <glm/glm.hpp>
#include <string>

class SpotLight : public Light {
  glm::vec3 direction;
  float cutOff = 12.5f;

public:
  SpotLight(glm::vec3 position_, float intensity_, glm::vec3 color_,
            glm::vec3 direction_, float curOff_) {
    setPosition(position_);
    setIntensity(intensity_);
    setColor(color_);
    setDiffuse(getDiffuse() * color_);
    setSpecular(getDiffuse() * color_);
    direction = direction_;
    cutOff = curOff_;
  }

  glm::vec3 getDirection() { return direction; }
  float getCutOff() { return cutOff; }

  std::string getType() override { return "SpotLight"; }
};
