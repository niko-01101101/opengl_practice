
#include "shader.h"
#include <iostream>
#include <map>
#include <string>

class ShaderManager {
private:
  std::map<std::string, Shader> shaders;

public:
  void loadShader(const std::string &name, const std::string &vertexPath,
                  const std::string &fragmentPath) {
    shaders.emplace(name, Shader(vertexPath.c_str(), fragmentPath.c_str()));
  }

  Shader *getShader(const std::string &name) {
    auto it = shaders.find(name);
    if (it != shaders.end()) {
      return &it->second; // Return a pointer to the shader
    }
    std::cerr << "Shader " << name << " not found!" << std::endl;
    return nullptr; // Return nullptr if not found
  }
};
