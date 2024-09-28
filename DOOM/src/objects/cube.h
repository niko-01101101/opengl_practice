#include "object.h"
#include <cstdio>
#include <glm/glm.hpp>

class Cube : public Object {
  std::array<float, 64> vertices;
  std::array<int, 36> indices = {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7,
                                 0, 3, 7, 0, 7, 4, 1, 5, 6, 1, 6, 2,
                                 0, 4, 5, 0, 5, 1, 3, 7, 6, 3, 6, 2};

  unsigned int texture;

public:
  Cube(glm::vec3 position_, glm::vec3 scale_) {
    vertices = {-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                1.0f,  -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                1.0f,  1.0f,  -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
                -1.0f, 1.0f,  -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                1.0f,  -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
                1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
                -1.0f, 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    setPosition(position_);
    setScale(scale_);
    setRotation(glm::vec3(45.0f, 45.0f, 0.0f));
  }

  Cube(glm::vec3 position) : Cube(position, glm::vec3(1.0f, 1.0f, 1.0f)) {}

  Cube() : Cube(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)) {}

  const float *getVertices() override { return vertices.data(); }
  const int *getIndices() override { return indices.data(); }
  size_t getVerticesSize() override { return vertices.size(); }
  size_t getIndicesSize() override { return indices.size(); }

  void Draw() override {
    setRotation(getRotation() + glm::vec3(0.0f, 0.0f, 0.0f));
  }
};
