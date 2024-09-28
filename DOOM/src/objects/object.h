#include <array>
#include <glm/glm.hpp>

class Object {
protected:
  glm::vec3 position;
  glm::vec3 scale;
  std::array<float, 32> vertices;
  std::array<int, 6> indices = {0, 1, 3, 1, 2, 3};

public:
  const glm::vec3 getPosition() { return position; }
  void setPosition(glm::vec3 position_) { position = position_; }
  const glm::vec3 getScale() { return scale; }
  void setScale(glm::vec3 scale_) { scale = scale_; }
  virtual const float *getVertices() { return vertices.data(); }
  virtual const int *getIndices() { return indices.data(); }
  virtual size_t getVerticesSize() { return vertices.size(); }
  virtual size_t getIndicesSize() { return indices.size(); }
};
