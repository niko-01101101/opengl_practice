#include <cstdio>
#include <string>
#include <xlocale/_stdio.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "loadshaders.cpp"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "objects/cube.h"
#include "objects/plane.h"

#include "objects/camera.h"

float deltaTime = 0.0f;
float globalTime = 0.0f;
float lastFrame = 0.0f;

Object *objects[] = {
    new Cube(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.1f, 0.4f)),
    new Cube(glm::vec3(1.0f, 0.4f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f)),
    new Cube(glm::vec3(-3.0f, -0.2f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f))};

bool meshMode = false;

// Camera Controls
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
Camera *camera = new Camera();

void checkShaderCompileStatus(GLuint shader) {
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    fprintf(stderr, "Shader Compilation Error: %s\n", infoLog);
  }
}

void checkProgramLinkStatus(GLuint program) {
  GLint success;
  GLchar infoLog[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    fprintf(stderr, "Program Linking Error: %s\n", infoLog);
  }
}

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

class Key {
  int keyCode;
  bool pressed = false;

public:
  Key(int key_) { keyCode = key_; }

  int getKeyCode() { return keyCode; }

  bool getPressed() { return pressed; }
  void setPressed(bool pressed_) { pressed = pressed_; }
};

std::vector<Key> keys = {
    Key(GLFW_KEY_ESCAPE), Key(GLFW_KEY_W), Key(GLFW_KEY_S),
    Key(GLFW_KEY_A),      Key(GLFW_KEY_D), Key(GLFW_KEY_M),
};

static void processInput(GLFWwindow *window) {
  float cameraSpeed = 6.0f * deltaTime;

  for (int key = 0; key < keys.size(); ++key) {
    Key keyOn = keys[key];
    int keyCode = keyOn.getKeyCode();
    if (glfwGetKey(window, keyCode) == GLFW_PRESS) {
      if (!keyOn.getPressed()) {
        if (keyCode == GLFW_KEY_ESCAPE)
          glfwSetWindowShouldClose(window, GLFW_TRUE);
        if (keyCode == GLFW_KEY_M)
          meshMode = !meshMode;
      }
      if (keyCode == GLFW_KEY_W)
        camera->setPosition(camera->getPosition() +
                            camera->getForward() * cameraSpeed);
      if (keyCode == GLFW_KEY_S)
        camera->setPosition(camera->getPosition() +
                            -camera->getForward() * cameraSpeed);
      if (keyCode == GLFW_KEY_A)
        camera->setPosition(camera->getPosition() +
                            -camera->getRight() * cameraSpeed);
      if (keyCode == GLFW_KEY_D)
        camera->setPosition(camera->getPosition() +
                            camera->getRight() * cameraSpeed);

      keyOn.setPressed(true);
    } else {
      keyOn.setPressed(false);
    }
  }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  camera->setForward(glm::normalize(direction));
}

unsigned int GetTexture(const char *path) {
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // Set the texture wrapping/filtering options (on the currently bound texture)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_set_flip_vertically_on_load(true);

  // Load and generate the texture
  int width, height, nrChannels;
  unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
  if (data) {
    GLenum format;
    if (nrChannels == 1)
      format = GL_RED;
    else if (nrChannels == 3)
      format = GL_RGB;
    else if (nrChannels == 4)
      format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    printf("Failed to load texture.\n");
  }
  stbi_image_free(data);

  return texture;
}

int main(void) {
  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  GLFWwindow *window = glfwCreateWindow(640, 480, "DOOM", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwSetCursorPosCallback(window, mouse_callback);

  // Initialize glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    fprintf(stderr, "Failed to initialize GLAD\n");
    exit(EXIT_FAILURE);
  }

  // Load Objects
  std::vector<float> vertices;
  std::vector<int> indices;
  int lastIndiCount = 0;
  for (Object *object : objects) {
    const int vertStartSize = vertices.size() / 8;

    const float *verts = object->getVertices();
    size_t vertSize = object->getVerticesSize();
    vertices.insert(vertices.end(), verts, verts + vertSize);

    const int *indis = object->getIndices();
    size_t indisSize = object->getIndicesSize();

    object->setID(lastIndiCount);

    for (size_t i = 0; i < indisSize; ++i) {
      indices.push_back(indis[i] + vertStartSize);
    }
    lastIndiCount = indisSize;
  }

  // Set up buffer
  unsigned int VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);

  glGenBuffers(1, &VBO);

  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int),
               indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // Color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // Texture coordinate attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Load shader program
  GLuint shaderProgram = glCreateProgram();

  // Load Texture
  unsigned int texture = GetTexture("textures/smile.png");

  // Load and compile the vertex shader
  const std::string vertexShaderRef = LoadShader("shaders/vertexShader.glsl");
  const char *vertexShaderSrc = vertexShaderRef.c_str();
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
  glCompileShader(vertexShader);
  checkShaderCompileStatus(vertexShader);

  const std::string redShaderRef = LoadShader("shaders/redFragShader.glsl");
  const char *redShader = redShaderRef.c_str();
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &redShader, NULL);
  glCompileShader(fragmentShader);
  checkShaderCompileStatus(fragmentShader);

  // Link shaders to a shader program
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  checkProgramLinkStatus(shaderProgram);

  // Clean up shaders as they are no longer needed
  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);

  glUseProgram(shaderProgram);

  glClearColor(0.0f, 0.2f, 0.4f, 1.0f);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    // RUNNING Draw
    if (meshMode)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // GLM Perspective
    globalTime = glfwGetTime();
    deltaTime = globalTime - lastFrame;

    // Camera
    glm::mat4 view = glm::mat4(1.0f);

    view = glm::translate(view, camera->getPosition());
    glm::vec3 rot = camera->getRotation();
    view = glm::rotate(view, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::rotate(view, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
    view = camera->LookAt(camera->getPosition() + camera->getForward());

    glm::mat4 projection;
    projection = camera->getPerspective();

    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    unsigned int projectionLoc =
        glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);

    // Update Objects
    glm::mat4 model = glm::mat4(1.0f);

    for (Object *object : objects) {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, object->getPosition());

      glm::vec3 rot = object->getRotation();
      model =
          glm::rotate(model, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
      model =
          glm::rotate(model, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
      model =
          glm::rotate(model, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

      model = glm::scale(model, object->getScale());

      unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

      size_t offset = object->getID() * sizeof(unsigned int);
      glDrawElements(GL_TRIANGLES, object->getIndicesSize(), GL_UNSIGNED_INT,
                     (void *)offset);

      object->Draw();
    }

    glfwSwapBuffers(window);
    glfwPollEvents();

    lastFrame = globalTime;
  }

  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
  glDeleteProgram(shaderProgram);
  glBindVertexArray(0);

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
