#include <glm/glm.hpp>
#include <vector>

#pragma once

class Object {
protected:
  glm::vec3 color = glm::vec3(1.0f);
  glm::vec3 position;
  glm::vec3 scale;
  glm::vec3 rotation;
  std::vector<float> vertices;
  std::vector<int> indices = {0, 1, 3, 1, 2, 3};
  unsigned int texture = 0;
  std::string shader = "unlitShader";
  unsigned int VAO;
  unsigned int VBO;
  unsigned int EBO;
  int material;

public:
  const glm::vec3 getPosition() { return position; }
  void setPosition(glm::vec3 position_) { position = position_; }
  const glm::vec3 getScale() { return scale; }
  void setScale(glm::vec3 scale_) { scale = scale_; }
  const glm::vec3 getRotation() { return rotation; }
  void setRotation(glm::vec3 rotation_) { rotation = rotation_; }

  unsigned int getTexture() { return texture; }
  void setTexture(unsigned int texture_) { texture = texture_; }

  int getMaterial() { return material; }
  void setMaterial(int material_) { material = material_; }

  glm::vec3 getColor() { return color; }
  void setColor(glm::vec3 color_) { color = color_; }

  unsigned int getVAO() { return VAO; }
  void setVAO(unsigned int VAO_) { VAO = VAO_; }

  unsigned int getVBO() { return VBO; }
  void setVBO(unsigned int VBO_) { VBO = VBO_; }

  unsigned int getEBO() { return EBO; }
  void setEBO(unsigned int EBO_) { EBO = EBO_; }

  std::string getShader() { return shader; }
  void setShader(std::string shader_) { shader = shader_; }

  virtual const float *getVertices() { return vertices.data(); }
  virtual const int *getIndices() { return indices.data(); }
  virtual size_t getVerticesSize() { return vertices.size(); }
  virtual size_t getIndicesSize() { return indices.size(); }

  virtual void Draw() {};
};
