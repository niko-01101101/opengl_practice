#include "light.h"
#include <glm/glm.hpp>
#include <string>

class SpotLight : public Light {
  glm::vec3 direction;
  float cutOff = 12.5f;
  float outerCutOff = 17.5f;
  unsigned int depthMap;

public:
  SpotLight(glm::vec3 position_, float intensity_, glm::vec3 color_,
            glm::vec3 direction_, float cutOff_, float outerCutOff_) {
    setPosition(position_);
    setIntensity(intensity_);
    setColor(color_);
    setDiffuse(getDiffuse() * color_);
    setSpecular(getDiffuse() * color_);
    direction = direction_;
    cutOff = cutOff_;
    outerCutOff = outerCutOff_;
    setIntensity(2.0);
  }

  glm::vec3 getDirection() { return direction; }
  void setDirection(glm::vec3 direction_) { direction = direction_; }
  float getCutOff() { return cutOff; }
  float getOuterCutOff() { return outerCutOff; }

  unsigned int getDepthMap() { return depthMap; }
  void setDepthMap(unsigned int depthMap_) { depthMap = depthMap_; }

  std::string getType() override { return "SpotLight"; }
};
