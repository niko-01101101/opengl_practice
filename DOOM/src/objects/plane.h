#include "object.h"
#include <cstdio>
#include <glm/glm.hpp>

class Plane : public Object {
  std::array<float, 32> vertices;
  std::array<int, 6> indices = {0, 1, 3, 1, 2, 3};
  unsigned int texture;

public:
  Plane(glm::vec3 position_, glm::vec3 scale_) {
    vertices = {scale_.x,  scale_.y,  0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                scale_.x,  -scale_.y, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                -scale_.x, -scale_.y, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
                -scale_.x, scale_.y,  0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    setPosition(position_);
    setScale(scale_);
  }

  Plane(glm::vec3 position) : Plane(position, glm::vec3(1.0f, 1.0f, 1.0f)) {}

  Plane() : Plane(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)) {}

  const float *getVertices() override { return vertices.data(); }
  const int *getIndices() override { return indices.data(); }
  size_t getVerticesSize() override { return vertices.size(); }
  size_t getIndicesSize() override { return indices.size(); }
};