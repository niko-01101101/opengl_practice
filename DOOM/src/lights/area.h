#include "light.h"
#include <glm/glm.hpp>

class AreaLight : public Light {
  float distance = 5.0f;

public:
  AreaLight(glm::vec3 position_, float intensity_, float distance_,
            glm::vec3 color_) {
    setPosition(position_);
    setIntensity(intensity_);
    setColor(color_);
    setDiffuse(getDiffuse() * color_);
    setSpecular(getDiffuse() * color_);
    distance = distance_;
  }

  std::string getType() override { return "AreaLight"; }
};
