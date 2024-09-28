#include <fstream>
#include <sstream>
#include <string>

// Utility function to load shader from file
std::string LoadShader(const std::string &filePath) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    fprintf(stderr, "Failed to open shader file: %s\n", filePath.c_str());
    exit(EXIT_FAILURE);
  }
  std::stringstream buffer;
  buffer << file.rdbuf(); // Read the file contents into the buffer
  return buffer.str();
}
