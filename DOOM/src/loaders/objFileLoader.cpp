#include "../objects/mesh.h"
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Vertex {
  float x, y, z, nx, ny, nz, tx, ty;
};

std::vector<glm::vec3> CalculateNormals(std::vector<int> indices,
                                        std::vector<Vertex> vertices) {
  std::vector<glm::vec3> vertexNormals(vertices.size(), glm::vec3(0.0f));

  for (int indexOn = 0; indexOn < indices.size(); indexOn += 3) {
    int i0 = indices[indexOn];
    int i1 = indices[indexOn + 1];
    int i2 = indices[indexOn + 2];

    glm::vec3 v0 = glm::vec3(vertices[i0].x, vertices[i0].y, vertices[i0].z);
    glm::vec3 v1 = glm::vec3(vertices[i1].x, vertices[i1].y, vertices[i1].z);
    glm::vec3 v2 = glm::vec3(vertices[i2].x, vertices[i2].y, vertices[i2].z);

    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;

    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
    vertexNormals[i0] += normal;
    vertexNormals[i1] += normal;
    vertexNormals[i2] += normal;
  }

  return vertexNormals;
}

Mesh *LoadObj(const std::string &filePath) {
  std::vector<Vertex> vertices;
  std::vector<int> indices;
  std::ifstream file(filePath);
  if (!file.is_open()) {
    fprintf(stderr, "Failed to open obj file: %s\n", filePath.c_str());
    exit(EXIT_FAILURE);
  }

  std::string line;
  int normalOn = 0;
  int texCoordOn = 0;
  while (std::getline(file, line)) {
    std::istringstream iss(line);

    if (line.substr(0, 2) == "v ") {
      Vertex vertex;
      char prefix;
      iss >> prefix >> vertex.x >> vertex.y >> vertex.z;
      vertices.push_back(vertex);
    } else if (line.substr(0, 2) == "vn ") {
      glm::vec3 normal;
      char prefix;
      iss >> prefix >> normal.x >> normal.y >> normal.z;
      vertices[normalOn].nx = normal.x;
      vertices[normalOn].ny = normal.y;
      vertices[normalOn].nz = normal.z;

      normalOn++;
    } else if (line.substr(0, 2) == "vt ") {
      glm::vec2 texCoord;
      char prefix;
      iss >> prefix >> texCoord.x >> texCoord.y;
      vertices[texCoordOn].tx = texCoord.x;
      vertices[texCoordOn].ty = texCoord.y;
      texCoordOn++;
    } else if (line.substr(0, 2) == "f ") {

      std::string vertexIndexStr;
      std::istringstream vertexStream(line.substr(2));

      while (vertexStream >> vertexIndexStr) {
        std::istringstream indexStream(vertexIndexStr);
        std::string indexStr;

        std::getline(indexStream, indexStr, '/');

        if (!indexStr.empty()) {
          try {
            int vertexIndex = std::stoi(indexStr) - 1;
            indices.push_back(vertexIndex);
          } catch (const std::invalid_argument &e) {
            std::cerr << "Invalid vertex index: " << indexStr
                      << " in line: " << line << std::endl;
          }
        } else {
          std::cerr << "Warning: Empty vertex index found in line: " << line
                    << std::endl;
        }
      }
    }
  }

  std::vector<float> outVertices;
  std::vector<glm::vec3> normals = CalculateNormals(indices, vertices);

  int vertexOn = 0;

  for (const Vertex &vertex : vertices) {

    glm::vec3 calcNormal = normals[vertexOn];

    outVertices.push_back(vertex.x);
    outVertices.push_back(vertex.y);
    outVertices.push_back(vertex.z);
    outVertices.push_back(calcNormal.x);
    outVertices.push_back(calcNormal.y);
    outVertices.push_back(calcNormal.z);
    outVertices.push_back(vertex.tx);
    outVertices.push_back(vertex.ty);

    vertexOn++;
  }

  return new Mesh(outVertices, indices, glm::vec3(0.0f, -5.0f, -3.0f),
                  glm::vec3(0.01f));
}
