#include "../objects/mesh.h"
#include <cstdlib>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Vertex {
  float x, y, z, nx, ny, nz, u, v;

  Vertex(){

  }
};

std::vector<glm::vec3> CalculateNormals(std::vector<int> indices,
                                        std::vector<Vertex> vertices) {
  std::vector<glm::vec3> vertexNormals(vertices.size(), glm::vec3(0.0f));

  for (int indexOn = 0; indexOn < indices.size(); indexOn += 3) {
    int i0 = indices[indexOn];
    int i1 = indices[indexOn + 1];
    int i2 = indices[indexOn + 2];

    if (i0 >= vertices.size() || i1 >= vertices.size() ||
        i2 >= vertices.size() || i0 < 0 || i1 < 0 || i2 < 0) {
      std::cerr << "Error: Invalid index in indices array." << std::endl;
      return {};
    }

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

Mesh *LoadObj(const std::string &filePath, glm::vec3 position, glm::vec3 scale,
              glm::vec3 rotation, unsigned int texture, std::string shader) {
  std::vector<Vertex> vertices;
  std::vector<glm::vec3> verticesPositions;

  std::vector<int> indices;
  std::vector<glm::vec2> texCoords;
  std::vector<glm::vec3> normals;
  std::ifstream file(filePath);
  if (!file.is_open()) {
    fprintf(stderr, "Failed to open obj file: %s\n", filePath.c_str());
    exit(EXIT_FAILURE);
  }

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);

    if (line.substr(0, 2) == "v ") {
      glm::vec3 vertex;
      char prefix;
      iss >> prefix >> vertex.x >> vertex.y >> vertex.z;
      verticesPositions.push_back(vertex);
    } else if (line.substr(0, 2) == "vn ") {
      glm::vec3 normal;
      char prefix;
      iss >> prefix >> normal.x >> normal.y >> normal.z;
      normals.push_back(normal);
    } else if (line.substr(0, 2) == "vt") {
      glm::vec2 texCoord;

      std::string prefix;
      iss >> prefix >> texCoord.x >> texCoord.y;

      texCoords.push_back(texCoord);
    } else if (line.substr(0, 2) == "f ") {

      std::string vertexIndexStr;
      std::istringstream vertexStream(line.substr(2));

      std::vector<int> faceIndices;

      while (vertexStream >> vertexIndexStr) {
        std::istringstream indexStream(vertexIndexStr);
        std::string indexStr, texStr, normStr;

        std::getline(indexStream, indexStr, '/');
        std::getline(indexStream, texStr, '/');
        std::getline(indexStream, normStr, '/');

        try {
          int vertexIndex = std::stoi(indexStr) - 1;
          faceIndices.push_back(vertices.size());

          Vertex vertex = Vertex();

          vertex.x = verticesPositions[vertexIndex].x;
          vertex.y = verticesPositions[vertexIndex].y;
          vertex.z = verticesPositions[vertexIndex].z;

          if (texCoords.size() > 0) {
            int texIndex = std::stoi(texStr) - 1;
            vertex.u = texCoords[texIndex].x;
            vertex.v = texCoords[texIndex].y;
          }

          if (normals.size() > 0) {
            int normIndex = std::stoi(normStr) - 1;
            vertex.nx = normals[normIndex].x;
            vertex.ny = normals[normIndex].y;
            vertex.nz = normals[normIndex].z;
          }

          vertices.push_back(vertex);

        } catch (const std::invalid_argument &e) {
          std::cerr << "Invalid vertex index: " << indexStr
                    << " in line: " << line << std::endl;
        }
      }

      if (faceIndices.size() == 4) {
        // Process as a quad (split into two triangles)
        indices.push_back(faceIndices[0]);
        indices.push_back(faceIndices[1]);
        indices.push_back(faceIndices[2]);

        indices.push_back(faceIndices[0]);
        indices.push_back(faceIndices[2]);
        indices.push_back(faceIndices[3]);
      } else if (faceIndices.size() == 3) {
        // Process as a triangle
        indices.push_back(faceIndices[0]);
        indices.push_back(faceIndices[1]);
        indices.push_back(faceIndices[2]);
      } else {
        std::cerr << "Warning: Face has " << faceIndices.size()
                  << " vertices, not 3 or 4." << std::endl;
      }
    }
  }

  std::vector<float> outVertices;
  bool normalsIncluded = normals.size() > 0;
  if (!normalsIncluded)
    normals = CalculateNormals(indices, vertices);

  int vertexOn = 0;
  std::cout << "TS: " << texCoords.size() << " VS: " << vertices.size()
            << " IS: " << indices.size() << "\n";

  for (const Vertex &vertex : vertices) {

    glm::vec3 calcNormal = normalsIncluded
                               ? glm::vec3(vertex.nx, vertex.ny, vertex.nz)
                               : normals[vertexOn];

    // Funny
    // float off = rand() / 1000000000.0f * 0.2f;
    outVertices.push_back(vertex.x);
    outVertices.push_back(vertex.y);
    outVertices.push_back(vertex.z);
    outVertices.push_back(calcNormal.x);
    outVertices.push_back(calcNormal.y);
    outVertices.push_back(calcNormal.z);
    outVertices.push_back(vertex.u);
    outVertices.push_back(vertex.v);

    vertexOn++;
  }

  return new Mesh(outVertices, indices, position, scale, rotation, texture, shader);
}
