#include "object.h"
#include <glm/glm.hpp>

class Mesh : public Object {
  std::vector<float> vertices = {
      1.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f, 0.0f,
      0.0f, 0.0f, 1.0f,  1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  1.0f,
      0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 1.0f};

  std::vector<int> indices = {0, 1, 3, 1, 2, 3};
  unsigned int texture;

public:
  Mesh(std::vector<float> vertices_, std::vector<int> indices_) {
    vertices = vertices_;
    indices = indices_;
    setShader("litShader");
    setMaterial(0);
  };

  Mesh(std::vector<float> vertices_, std::vector<int> indices_,
       glm::vec3 position_, glm::vec3 scale_)
      : Mesh(vertices_, indices_) {
    setScale(scale_);
    setPosition(position_);
  };

  const float *getVertices() override { return vertices.data(); }
  const int *getIndices() override { return indices.data(); }
  size_t getVerticesSize() override { return vertices.size(); }
  size_t getIndicesSize() override { return indices.size(); }

  void Draw() override {
    setRotation(getRotation() + glm::vec3(0.0f, 1.0f, 0.0f));
  }
};