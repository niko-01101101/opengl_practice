#include "objects/object.h"
#include <cstdio>
#include <string>
#include <xlocale/_stdio.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>

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

#include "loaders/cubeMapLoader.cpp"
#include "loaders/objFileLoader.cpp"

#include "material.h"

float deltaTime = 0.0f;
float globalTime = 0.0f;
float lastFrame = 0.0f;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Object *objects[] = {
    new Plane(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(10, 10, 10),
              glm::vec3(-90, 0, 0), 3, "litShader"),
    new Cube(glm::vec3(-4, -1.2f, -0.3f), glm ::vec3(1), 6, glm::vec3(1),
             "litShader", 1),
    LoadObj("models/sphere.obj", glm::vec3(3, 0.3, -0.5), glm::vec3(1),
            glm::vec3(0), 0, "litShader"),
    LoadObj("models/skull.obj", glm::vec3(0), glm::vec3(0.1f),
            glm::vec3(-90, -23, 50), 1, "litShader"),
};

float ambient = 0.2f;
Light *lights[] = {new SpotLight(glm::vec3(10.0f, 10.0f, 10.0f), 5.0f,
                                 glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-45, -45, 0), 90.0f, 92.5f),
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
SmoothCamera *camera = new SmoothCamera(SCR_WIDTH, SCR_HEIGHT);

float skyboxVertices[] = {
    // positions
    -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
    1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

    -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
    -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

    1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
    1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

glm::vec3 eulerToDirection(float pitch, float yaw) {
  // Convert degrees to radians
  float pitchRad = glm::radians(pitch);
  float yawRad = glm::radians(yaw);

  // Calculate the direction vector
  glm::vec3 direction;
  direction.x = cos(pitchRad) * sin(yawRad);
  direction.y = sin(pitchRad);
  direction.z = cos(pitchRad) * cos(yawRad);

  return glm::normalize(direction);
}

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

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

int main(void) {
  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Renderer", NULL, NULL);
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

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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
  shaderManager.loadShader("litShader", "shaders/vertexShader.vs",
                           "shaders/litShader.fs");
  shaderManager.loadShader("unlitShader", "shaders/vertexShader.vs",
                           "shaders/unlitShader.fs");
  shaderManager.loadShader("depthShader", "shaders/depthVertexShader.vs",
                           "shaders/depthFragShader.fs");
  shaderManager.loadShader("skyboxShader", "shaders/skyboxShader.vs",
                           "shaders/skyboxShader.fs");

  // Load Materials
  Material materials[] = {Material(0, 0, glm::vec3(1.0f, 1.0f, 1.0f), 0.3),
                          Material(6, 7, glm::vec3(1.0f, 1.0f, 1.0f), 1.0)};

  // Load Texture
  unsigned int skullTexture = GetTexture("textures/skull.jpg");
  unsigned int smileTexture = GetTexture("textures/smile.png");
  unsigned int brickTexture = GetTexture("textures/bricks.jpg");
  unsigned int logoTexture = GetTexture("textures/logo.png");
  unsigned int metalCrateTexture = GetTexture("textures/metal-crate.jpg");
  unsigned int crateTexture = GetTexture("textures/crate/crate.png");
  unsigned int crateSpecularTexture =
      GetTexture("textures/crate/crate-rim.png");
  unsigned int skyboxTexture = GetTexture("textures/skybox.jpg");

  /* std::vector<std::string> faces{
       "textures/simple_skybox/right.tga", "textures/simple_skybox/left.tga",
       "textures/simple_skybox/top.tga",   "textures/simple_skybox/bottom.tga",
       "textures/simple_skybox/front.tga", "textures/simple_skybox/back.tga"};*/
  std::vector<std::string> faces{
      "textures/ex_skybox/right.jpg",  "textures/ex_skybox/left.jpg",
      "textures/ex_skybox/bottom.jpg", "textures/ex_skybox/top.jpg",
      "textures/ex_skybox/front.jpg",  "textures/ex_skybox/back.jpg"};
  unsigned int skybox = loadCubemap(faces);

  // skybox VAO
  unsigned int skyboxVAO, skyboxVBO;
  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);
  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

  for (Light *light : lights) {
    if (light->getType() == "SpotLight") {

      SpotLight *spotlight = dynamic_cast<SpotLight *>(light);

      unsigned int depthMapFBO;
      glGenFramebuffers(1, &depthMapFBO);
      spotlight->setDepthMapFBO(depthMapFBO);

      unsigned int depthMap;
      glGenTextures(1, &depthMap);
      glBindTexture(GL_TEXTURE_2D, depthMap);
      spotlight->setDepthMap(depthMap);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
                   SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      float borderColor[] = {1.0, 1.0, 1.0, 1.0};
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
      // attach depth texture as FBO's depth buffer
      glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                             depthMap, 0);
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
    }
  }

  // Check framebuffer completeness
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Framebuffer not complete!" << std::endl;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glFrontFace(GL_CW);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    // RUNNING Draw
    if (meshMode)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ShadowMap Creation
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 1.0f, far_plane = 100.0f;
    lightProjection = glm::perspective(
        glm::radians(90.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT,
        near_plane, far_plane);

    // lightProjection =
    //    glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    for (Light *light : lights) {

      if (light->getType() == "SpotLight") {

        SpotLight *spotlight = dynamic_cast<SpotLight *>(light);

        lightView = glm::mat4();

        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 lightRot = spotlight->getDirection();
        glm::vec3 direction = eulerToDirection(-lightRot.y, -lightRot.x);

        lightView = glm::lookAt(spotlight->getPosition(),
                                (spotlight->getPosition() - direction), up);

        lightSpaceMatrix = lightProjection * lightView;

        spotlight->setSpaceMatrix(lightSpaceMatrix);

        glBindFramebuffer(GL_FRAMEBUFFER, spotlight->getDepthMapFBO());
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glClear(GL_DEPTH_BUFFER_BIT);

        Shader *depthShader = shaderManager.getShader("depthShader");
        depthShader->use();
        depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

        // Quick Render
        for (Object *object : objects) {
          glm::mat4 model = glm::mat4(1.0f);
          model = glm::translate(model, object->getPosition());

          glm::vec3 rot = object->getRotation();
          model = glm::rotate(model, glm::radians(rot.x),
                              glm::vec3(1.0f, 0.0f, 0.0f));
          model = glm::rotate(model, glm::radians(rot.y),
                              glm::vec3(0.0f, 1.0f, 0.0f));
          model = glm::rotate(model, glm::radians(rot.z),
                              glm::vec3(0.0f, 0.0f, 1.0f));

          model = glm::scale(model, object->getScale());

          depthShader->setMat4("model", model);

          glBindVertexArray(object->getVAO());

          glDrawElements(GL_TRIANGLES, object->getIndicesSize(),
                         GL_UNSIGNED_INT, 0);

          glBindVertexArray(0);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }
    }

    // reset viewport
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    float aspectRatio = (float)width / (float)height;
    glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // GLM Perspective
    globalTime = glfwGetTime();
    deltaTime = globalTime - lastFrame;

    // Camera
    glm::mat4 view = glm::mat4(1.0f);

    camera->setPerspective(45.0f, aspectRatio, 0.1f, 300.0f);

    camera->calc();

    view = glm::translate(view, camera->getPosition());
    view = camera->LookAt(camera->getPosition() + camera->getForward());

    glm::mat4 projection;

    projection = camera->getPerspective();

    // Render Skybox
    glDepthMask(GL_FALSE);
    Shader *skyboxShader = shaderManager.getShader("skyboxShader");
    skyboxShader->use();
    skyboxShader->setMat4("view", glm::mat4(glm::mat3(view)));
    skyboxShader->setMat4("projection", projection);
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);

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

      currentShader->setVec3("ambient", glm::vec3(ambient));

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

          int depthTex = (GL_TEXTURE0 + lightId);
          glActiveTexture(depthTex);
          glBindTexture(GL_TEXTURE_2D, spotlight->getDepthMap());

          currentShader->setInt(lightStr + ".depthMapId", lightId);
          currentShader->setInt("depthMaps[" + std::to_string(lightId) + "]",
                                lightId);

          currentShader->setMat4(lightStr + ".lightSpaceMatrix",
                                 spotlight->getSpaceMatrix());

          currentShader->setVec3(lightStr + ".direction",
                                 spotlight->getDirection());
          currentShader->setFloat(
              lightStr + ".cutOff",
              glm::cos(glm::radians(spotlight->getCutOff())));
          currentShader->setFloat(
              lightStr + ".outerCutOff",
              glm::cos(glm::radians(spotlight->getOuterCutOff())));
          // TODO: Change to use spotlight vars
          currentShader->setFloat(lightStr + ".constant", 1.0f);
          currentShader->setFloat(lightStr + ".linear", 0.09f);
          currentShader->setFloat(lightStr + ".quadratic", 0.032f);
        } else {
          currentShader->setInt(lightStr + ".type", 0);
        }

        lightId++;
      }
      currentShader->setMat4("projection", projection);
      currentShader->setMat4("view", view);

      glBindVertexArray(object->getVAO());

      glActiveTexture(GL_TEXTURE0 + lightId);
      glBindTexture(GL_TEXTURE_2D, object->getTexture());
      currentShader->setInt("material.texture", lightId);

      glActiveTexture(GL_TEXTURE1 + lightId);
      glBindTexture(GL_TEXTURE_2D, currentMaterial.getDiffuse());
      currentShader->setInt("material.diffuse", lightId + 1);

      glActiveTexture(GL_TEXTURE2 + lightId);
      glBindTexture(GL_TEXTURE_2D, currentMaterial.getSpecular());
      currentShader->setInt("material.specular", lightId + 2);

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

  glDeleteVertexArrays(1, &skyboxVAO);
  glDeleteBuffers(1, &skyboxVBO);

  glBindVertexArray(0);

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
