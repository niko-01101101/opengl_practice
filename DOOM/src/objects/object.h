#include <array>
#include <glm/glm.hpp>

#pragma once

class Object {
protected:
  glm::vec3 position;
  glm::vec3 scale;
  glm::vec3 rotation;
  std::array<float, 32> vertices;
  std::array<int, 6> indices = {0, 1, 3, 1, 2, 3};
  int id;

public:
  const glm::vec3 getPosition() { return position; }
  void setPosition(glm::vec3 position_) { position = position_; }
  const glm::vec3 getScale() { return scale; }
  void setScale(glm::vec3 scale_) { scale = scale_; }
  const glm::vec3 getRotation() { return rotation; }
  void setRotation(glm::vec3 rotation_) { rotation = rotation_; }

  virtual const float *getVertices() { return vertices.data(); }
  virtual const int *getIndices() { return indices.data(); }
  virtual size_t getVerticesSize() { return vertices.size(); }
  virtual size_t getIndicesSize() { return indices.size(); }

  virtual void Draw(){};

  const int getID() { return id; }
  void setID(int id_) { id = id_; }
};
