#include "object.h"
#include <cstdio>
#include <glm/glm.hpp>
#include <vector>

class Cube : public Object {
  std::vector<float> vertices = {
      // Positions          // Normals            // Texture Coords
      // Front face
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom-left
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // Bottom-right
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // Top-right
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // Top-left

      // Back face
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
      0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // Bottom-right
      0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // Top-right
      -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // Top-left

      // Left face
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Bottom-left
      -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // Bottom-right
      -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // Top-right
      -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // Top-left

      // Right face
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Bottom-left
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // Bottom-right
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // Top-right
      0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // Top-left

      // Bottom face
      -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Bottom-left
      0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // Bottom-right
      0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,   // Top-right
      -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,  // Top-left

      // Top face
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // Bottom-left
      0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // Bottom-right
      0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,   // Top-right
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Top-left
  };

  std::vector<int> indices = {
      0,  1,  2,  0,  2,  3,  // Front face
      4,  6,  5,  4,  7,  6,  // Back face
      8,  9,  10, 8,  10, 11, // Left face
      12, 14, 13, 12, 15, 14, // Right face
      16, 17, 18, 16, 18, 19, // Top face
      20, 22, 21, 20, 23, 22  // Bottom face
  };

  unsigned int texture;

public:
  Cube(glm::vec3 position_, glm::vec3 scale_, int texture_) {
    setPosition(position_);
    setScale(scale_);
    setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    setTexture(texture_);
  }

  Cube(glm::vec3 position_, glm::vec3 scale_, int texture_, glm::vec3 color_)
      : Cube(position_, scale_, texture_) {
    setColor(color_);
  }

  Cube(glm::vec3 position_, glm::vec3 scale_, int texture_, std::string shader_)
      : Cube(position_, scale_, texture_) {
    setShader(shader_);
  }

  Cube(glm::vec3 position_, glm::vec3 scale_, int texture_, glm::vec3 color_,
       std::string shader_)
      : Cube(position_, scale_, texture_, color_) {
    setShader(shader_);
  }

  Cube(glm::vec3 position_, glm::vec3 scale_, int texture_, glm::vec3 color_,
       std::string shader_, int material_)
      : Cube(position_, scale_, texture_, color_, shader_) {
    setMaterial(material_);
  }

  Cube(glm::vec3 position, int texture_)
      : Cube(position, glm::vec3(1.0f, 1.0f, 1.0f), texture_) {}

  Cube() : Cube(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0) {}

  const float *getVertices() override { return vertices.data(); }
  const int *getIndices() override { return indices.data(); }
  size_t getVerticesSize() override { return vertices.size(); }
  size_t getIndicesSize() override { return indices.size(); }

  void Draw() override {
    //setRotation(getRotation() + glm::vec3(0.5f, 1.0f, 0.0f));
  }
};
