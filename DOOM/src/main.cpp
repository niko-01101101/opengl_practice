#include "objects/object.h"
#include <cstdio>
#include <string>
#include <xlocale/_stdio.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "objects/cube.h"
#include "objects/plane.h"

#include "objects/camera.h"
#include "objects/smoothCamera.h"

#include "lights/area.h"
#include "lights/light.h"
#include "lights/spot.h"

#include "shader.h"
#include "shaderManager.h"

#include "loaders/objFileLoader.cpp"
#include "material.h"

float deltaTime = 0.0f;
float globalTime = 0.0f;
float lastFrame = 0.0f;

Object *objects[] = {
    new Cube(glm::vec3(0.0f, -3.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), 0,
             "unlitShader"),
    new Plane(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(10, 10, 10),
              glm::vec3(-90, 0, 0), 3, "litShader"),
    LoadObj("models/skull.obj", glm::vec3(3, -3.0, 0), glm::vec3(0.2f),
            glm::vec3(-90, 0, 0), 1),
    LoadObj("models/cat.obj", glm::vec3(-3, 0, 0), glm::vec3(0.01f),
            glm::vec3(0, 0, 0), 0),
};

Light *lights[] = {new SpotLight(glm::vec3(0.0f, 3.0f, 0.0f), 1.0f,
                                 glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0, 1, 0), 12.5f),
                   // Colored Lights
                   /*new AreaLight(glm::vec3(-5.0f, 3.0f, 0.0f), 1.0f, 10.0f,
                                 glm::vec3(0.0f, 0.0f, 1.0f)),
                   new AreaLight(glm::vec3(0.0f, -3.0f, 5.0f), 1.0f, 10.0f,
                                 glm::vec3(0.0f, 1.0f, 0.0f)),
                   new AreaLight(glm::vec3(5.0f, 0.0f, 0.0f), 1.0f, 10.0f,
                                 glm::vec3(1.0f, 0.0f, 0.0f))*/};

bool meshMode = false;

// Camera Controls
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
SmoothCamera *camera = new SmoothCamera();

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
        camera->addVelocity(camera->getForward() * cameraSpeed);

      if (keyCode == GLFW_KEY_S)
        camera->addVelocity(-camera->getForward() * cameraSpeed);

      if (keyCode == GLFW_KEY_A)
        camera->addVelocity(-camera->getRight() * cameraSpeed);

      if (keyCode == GLFW_KEY_D)
        camera->addVelocity(camera->getRight() * cameraSpeed);

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

int textureId = 0;
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

    glActiveTexture(GL_TEXTURE + textureId);
    textureId++;
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
  GLFWwindow *window = glfwCreateWindow(640, 480, "Renderer", NULL, NULL);
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

  // Load objects' verticies
  int currentId = 1;
  for (Object *object : objects) {
    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, object->getVerticesSize() * sizeof(float),
                 object->getVertices(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 object->getIndicesSize() * sizeof(int), object->getIndices(),
                 GL_STATIC_DRAW);

    object->setVAO(VAO);
    object->setVBO(VBO);
    object->setEBO(EBO);
    currentId++;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
  }

  // Load shader
  ShaderManager shaderManager;
  shaderManager.loadShader("litShader", "shaders/vertexShader.glsl",
                           "shaders/litShader.glsl");
  shaderManager.loadShader("unlitShader", "shaders/vertexShader.glsl",
                           "shaders/unlitShader.glsl");

  // Load Materials
  Material materials[] = {Material(0, 0, glm::vec3(1.0f, 1.0f, 1.0f), 0.1),
                          Material(5, 6, glm::vec3(1.0f, 1.0f, 1.0f), 1.0)};

  // Load Texture
  unsigned int skullTexture = GetTexture("textures/skull.jpg");
  unsigned int smileTexture = GetTexture("textures/smile.png");
  unsigned int brickTexture = GetTexture("textures/bricks.jpg");
  unsigned int logoTexture = GetTexture("textures/logo.png");
  unsigned int metalCrateTexture = GetTexture("textures/metal-crate.jpg");
  unsigned int crateTexture = GetTexture("textures/crate/crate.png");
  unsigned int crateSpecularTexture =
      GetTexture("textures/crate/crate-rim.png");

  glClearColor(0.0f, 0.2f, 0.4f, 1.0f);

  // Shadow Camera
  glm::mat4 lightProjection, lightView;
  glm::mat4 lightSpaceMatrix;
  float near_plane = 1.0f, far_plane = 7.5f;
  lightProjection =
      glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

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
    camera->calc();

    view = glm::translate(view, camera->getPosition());
    glm::vec3 rot = camera->getRotation();
    view = camera->LookAt(camera->getPosition() + camera->getForward());

    glm::mat4 projection;

    projection = camera->getPerspective();

    // Update Objects
    for (Object *object : objects) {
      Material currentMaterial = materials[object->getMaterial()];
      Shader *currentShader = shaderManager.getShader(object->getShader());
      currentShader->use();

      currentShader->setInt("material.diffuseId", currentMaterial.getDiffuse());
      currentShader->setInt("material.specularId",
                            currentMaterial.getSpecular());
      currentShader->setVec3("material.color", currentMaterial.getColor());
      currentShader->setFloat("material.shine", currentMaterial.getShine());

      // Load lights
      int lightId = 0;
      for (Light *light : lights) {
        std::string lightStr = ("lights[" + std::to_string(lightId) + "]");

        currentShader->setVec3(lightStr + ".diffuse", light->getDiffuse());
        currentShader->setVec3(lightStr + ".specular", light->getSpecular());
        currentShader->setVec3(lightStr + ".position", light->getPosition());
        currentShader->setFloat(lightStr + ".intensity", light->getIntensity());

        if (light->getType() == "SpotLight") {
          currentShader->setInt(lightStr + ".type", 1);

          SpotLight *spotlight = dynamic_cast<SpotLight *>(light);
          currentShader->setVec3(lightStr + ".direction",
                                 spotlight->getDirection());
          currentShader->setFloat(
              lightStr + ".cutOff",
              glm::cos(glm::radians(spotlight->getCutOff())));
          currentShader->setFloat("light.constant", 1.0f);
          currentShader->setFloat("light.linear", 0.09f);
          currentShader->setFloat("light.quadratic", 0.032f);
        } else {
          currentShader->setInt(lightStr + ".type", 0);
        }

        lightId++;
      }
      currentShader->setMat4("projection", projection);
      currentShader->setMat4("view", view);

      glBindVertexArray(object->getVAO());

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, currentMaterial.getDiffuse());
      currentShader->setInt("material.diffuse", 1);

      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, currentMaterial.getSpecular());
      currentShader->setInt("material.specular", 2);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, object->getTexture());
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

      currentShader->setMat4("model", model);
      currentShader->setVec3("viewPos", camera->getPosition());
      currentShader->setInt("textureId", object->getTexture());

      glDrawElements(GL_TRIANGLES, object->getIndicesSize(), GL_UNSIGNED_INT,
                     0);

      object->Draw();

      glBindVertexArray(0);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();

    lastFrame = globalTime;
  }

  // Cleanup
  // Delete Object Buffers
  for (Object *object : objects) {
    unsigned int vao = object->getVAO();
    unsigned int vbo = object->getVBO();
    unsigned int ebo = object->getEBO();

    if (vao != 0) {
      glDeleteVertexArrays(1, &vao);
    }
    if (vbo != 0) {
      glDeleteBuffers(1, &vbo);
    }
    if (ebo != 0) {
      glDeleteBuffers(1, &ebo);
    }
  }

  glBindVertexArray(0);

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
