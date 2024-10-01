#include <glm/glm.hpp>

class Material {
  unsigned int diffuse;
  unsigned int specular;
  glm::vec3 color;
  float shine;

public:
  Material(unsigned int diffuse_, unsigned int specular_, glm::vec3 color_,
           float shine_) {
    diffuse = diffuse_;
    specular = specular_;
    color = color_;
    shine = shine_;
  }

  unsigned int getDiffuse() { return diffuse; }
  unsigned int getSpecular() { return specular; }
  glm::vec3 getColor() { return color; }
  float getShine() { return shine; }
};
