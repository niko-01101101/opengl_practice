#include "object.h"
#include <cstdio>
#include <glm/glm.hpp>

class Sprite : public Object {
  std::array<float, 32> vertices = {
      1.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f, 0.0f,
      0.0f, 0.0f, 1.0f,  1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  1.0f,
      0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 1.0f};
  ;
  std::array<int, 6> indices = {0, 1, 3, 1, 2, 3};
  unsigned int texture;

public:
  Sprite(glm::vec3 position_, glm::vec3 scale_) {

    setPosition(position_);
    setScale(scale_);
    setRotation(glm::vec3(0.0001f));
  }

  Sprite(glm::vec3 position_, glm::vec3 scale_, glm::vec3 rotation_) {

    setPosition(position_);
    setScale(scale_);
    setRotation(rotation_);
  }

  Sprite(glm::vec3 position_, glm::vec3 scale_, glm::vec3 rotation_,
         unsigned int texture_) {
    setPosition(position_);
    setScale(scale_);
    setRotation(rotation_);
    setTexture(texture_);
  }

  Sprite(glm::vec3 position) : Sprite(position, glm::vec3(1.0f, 1.0f, 1.0f)) {}

  Sprite() : Sprite(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)) {}

  const float *getVertices() override { return vertices.data(); }
  const int *getIndices() override { return indices.data(); }
  size_t getVerticesSize() override { return vertices.size(); }
  size_t getIndicesSize() override { return indices.size(); }

  void Draw() override {
    setRotation(getRotation() + glm::vec3(0.0f, 0.0f, 0.0f));
  }
};
