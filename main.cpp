// ----------------------------------------------------------------------------
// main.cpp
//
//  Created on: 24 Jul 2020
//      Author: Kiwon Um
//        Mail: kiwon.um@telecom-paris.fr
//
// Description: IGR201 Practical; OpenGL and Shaders (DO NOT distribute!)
//
// Copyright 2020-2024 Kiwon Um
//
// The copyright to the computer program(s) herein is the property of Kiwon Um,
// Telecom Paris, France. The program(s) may be used and/or copied only with
// the written permission of Kiwon Um or in accordance with the terms and
// conditions stipulated in the agreement/contract under which the program(s)
// have been supplied.
// ----------------------------------------------------------------------------

#define _USE_MATH_DEFINES

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


// Constants
const static float kSizeSun = 2.3f;
const static float kSizeEarth = 0.7f;
const static float kSizeMoon = 0.2f;
const static float kSizeJupiter = 1.2f;
const static float kSizeSaturn = 1.0f;
const static float kSizeUranus = 0.9f;
const static float kSizeNeptune = 0.85f;
const static float kSizeVenus = 0.6f;
const static float kSizeMars = 0.5f;
const static float kSizeMercury = 0.4f;

const static float kRadOrbitEarth = 15.0f;
const static float kRadOrbitMoon = 3.0f;
const static float kRadOrbitJupiter = 25.0f;
const static float kRadOrbitSaturn = 30.0f;
const static float kRadOrbitUranus = 35.0f;
const static float kRadOrbitNeptune = 40.0f;
const static float kRadOrbitVenus = 10.0f;
const static float kRadOrbitMars = 18.0f;
const static float kRadOrbitMercury = 8.0f;

const float kRotatePeriodEarth = 10.0f;
const float kOrbitPeriodEarth = kRotatePeriodEarth * 5.0f;
const float kRotatePeriodMoon = kRotatePeriodEarth / 2.0f;
const float kOrbitPeriodMoon = kRotatePeriodMoon * 2.0f;
const float kRotatePeriodMercury = 12.0f;
const float kOrbitPeriodMercury = 24.0f; 
const float kRotatePeriodVenus = 9.0f;
const float kOrbitPeriodVenus = 18.0f;
const float kRotatePeriodMars = 6.0f;
const float kOrbitPeriodMars = 20.0f;
const float kRotatePeriodJupiter = 8.0f;
const float kOrbitPeriodJupiter = 20.0f;
const float kRotatePeriodSaturn = 9.0f;
const float kOrbitPeriodSaturn = 15.0f;
const float kRotatePeriodUranus = 10.0f;
const float kOrbitPeriodUranus = 40.0f;
const float kRotatePeriodNeptune = 9.0f;
const float kOrbitPeriodNeptune = 30.0f;


// Model transformation matrices
glm::mat4 g_sun, g_earth, g_moon;
const glm::vec3 sunPosition = glm::vec3(0.0, 0.0, 0.0);
const glm::vec3 earthPosition = glm::vec3(10.0 + kSizeSun/2, 0.0, 0.0);
const glm::vec3 moonPosition = glm::vec3(2.0 + kSizeEarth/2, 0.0, 0.0) + earthPosition; // Relative to the Earth

// Set up color for each object
const glm::vec3 sunColor = glm::vec3(1.0, 1.0, 1.0);
// Window parameters
GLFWwindow *g_window = nullptr;

// GPU objects
GLuint g_program = 0; // A GPU program contains at least a vertex shader and a fragment shader

// Basic camera model
class Camera {
public:
  inline float getFov() const { return m_fov; }
  inline void setFoV(const float f) { m_fov = f; }
  inline float getAspectRatio() const { return m_aspectRatio; }
  inline void setAspectRatio(const float a) { m_aspectRatio = a; }
  inline float getNear() const { return m_near; }
  inline void setNear(const float n) { m_near = n; }
  inline float getFar() const { return m_far; }
  inline void setFar(const float n) { m_far = n; }
  inline void setPosition(const glm::vec3 &p) { m_pos = p; }
  inline glm::vec3 getPosition() { return m_pos; }

  inline glm::mat4 computeViewMatrix() const {
    return glm::lookAt(m_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  }

  // Returns the projection matrix stemming from the camera intrinsic parameter.
  inline glm::mat4 computeProjectionMatrix() const {
    return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near, m_far);
  }
void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
        const float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        if (constrainPitch) {
            if (m_pitch > 89.0f) m_pitch = 89.0f;
            if (m_pitch < -89.0f) m_pitch = -89.0f;
        }

        updateCameraVectors();
    }

    void processMouseScroll(float yoffset) {
        m_fov -= yoffset;
        if (m_fov < 1.0f) m_fov = 1.0f;
        if (m_fov > 45.0f) m_fov = 45.0f;
    }

private:
    glm::vec3 m_pos = glm::vec3(0.0f, 0.0f, 30.0f);
    glm::vec3 m_front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 m_right = glm::vec3(1.0f, 0.0f, 0.0f);

    float m_yaw = -90.0f; // Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction to the right
    float m_pitch = 0.0f;
    float m_fov = 45.0f;  // Field of view, in degrees
    float m_aspectRatio = 1.0f;  // Ratio between the width and the height of the image
    float m_near = 0.1f; // Distance before which geometry is excluded from the rasterization process
    float m_far = 200.0f; // Distance after which geometry is excluded from the rasterization process

    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front = glm::normalize(front);
        m_right = glm::normalize(glm::cross(m_front, glm::vec3(0.0f, 1.0f, 0.0f)));
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }
};

Camera g_camera;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    g_camera.processMouseScroll(yoffset);
}

GLuint loadTextureFromFileToGPU(const std::string &filename) {
  int width, height, numComponents;
  // Loading the image in CPU memory using stb_image
  unsigned char *data = stbi_load(
    filename.c_str(),
    &width, &height,
    &numComponents, // 1 for a 8 bit grey-scale image, 3 for 24bits RGB image, 4 for 32bits RGBA image
    0);
  GLuint texID;
  glGenTextures(1, &texID); // generate an OpenGL texture container
  glBindTexture(GL_TEXTURE_2D, texID); // activate the texture
  // Setup the texture filtering option and repeat mode; check www.opengl.org for details.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // Fill the GPU texture with the data stored in the CPU image
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  // Free useless CPU memory
  stbi_image_free(data);
  // glBindTexture(GL_TEXTURE_2D, 0);
  return texID;
}
GLuint backgroundTex;
// Background
class Background {
public:
    void init() {
        // Define vertex positions for a fullscreen quad
        m_vertexPositions = {
            -100.0f, -100.0f, -50.0f,  // Bottom left
             100.0f, -100.0f, -50.0f,  // Bottom right
             100.0f,  100.0f, -50.0f,  // Top right
            -100.0f,  100.0f, -50.0f   // Top left
        };
        m_vertexTexCoords = {
            0.0f, 0.0f,  // Bottom left
            1.0f, 0.0f,  // Bottom right
            1.0f, 1.0f,  // Top right
            0.0f, 1.0f   // Top left
        };
        
        // Define indices for the quad
        m_triangleIndices = {
            0, 1, 2,
            0, 2, 3
        };
        // Generate and bind VAO, VBOs, and EBO
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        //Genereate a GPU buffer to store the positions of the vertices
        size_t vertexBufferSize = sizeof(float) * m_vertexPositions.size();
        glGenBuffers(1, &m_posVbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
        glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, m_vertexPositions.data(), GL_DYNAMIC_READ);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
        glEnableVertexAttribArray(0);
        //texture
        size_t textureBufferSize = sizeof(float) * m_vertexTexCoords.size();
        glGenBuffers(1, &m_texCoordVbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_texCoordVbo);
        glBufferData(GL_ARRAY_BUFFER, textureBufferSize, m_vertexTexCoords.data(), GL_DYNAMIC_READ);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);
        glEnableVertexAttribArray(2);

        //index
        size_t indexBufferSize = sizeof(unsigned int)*m_triangleIndices.size();
        glGenBuffers(1, &m_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, m_triangleIndices.data(), GL_DYNAMIC_READ);

        //deactivate the VAO, will be activated again when rendering
        glBindVertexArray(0);
    }

    void render() {
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_triangleIndices.size(), GL_UNSIGNED_INT, 0);
    }

private:
    std::vector<float> m_vertexPositions;
    std::vector<unsigned int> m_triangleIndices;
    std::vector<float> m_vertexTexCoords;

    GLuint m_vao = 0;
    GLuint m_posVbo = 0;
    GLuint m_texCoordVbo = 0;
    GLuint m_ibo = 0;
};
Background g_background;

class Mesh{
  public:
    void init(); // should properly set up the geometry buffer
    void render(); // should be called in the main rendering loop
    static std::shared_ptr<Mesh> genSphere(const size_t resolution=16); // should generate a unit sphere
    void update_model(glm::mat4 model){
      ModelMatrix = model;
    }
    void update_position(glm::vec3 newposition){
      position = newposition;
    }
    // ...
  private:
    std::vector<float> m_vertexPositions;
    std::vector<float> m_vertexNormals;
    std::vector<unsigned int> m_triangleIndices;
    std::vector<float> m_vertexTexCoords;
    GLuint m_vao = 0;
    GLuint m_posVbo = 0;
    GLuint m_normalVbo = 0;
    GLuint m_ibo = 0;
    GLuint m_texCoordVbo = 0;
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::vec3 position;
    // ...
};
auto sunMesh = std::make_shared<Mesh>();
auto moonMesh = std::make_shared<Mesh>();
auto earthMesh = std::make_shared<Mesh>();
auto mercuryMesh = std::make_shared<Mesh>();
auto venusMesh = std::make_shared<Mesh>();
auto marsMesh = std::make_shared<Mesh>();
auto jupiterMesh = std::make_shared<Mesh>();
auto saturnMesh = std::make_shared<Mesh>();
auto uranusMesh = std::make_shared<Mesh>();
auto neptuneMesh = std::make_shared<Mesh>();
GLuint sunTex;
GLuint earthTex;
GLuint moonTex;
GLuint mercuryTex;
GLuint venusTex;
GLuint marsTex;
GLuint jupiterTex;
GLuint saturnTex;
GLuint uranusTex;
GLuint neptuneTex;


void Mesh::init(){
    //Create a single handle, vertex array object that contains attributes,
    //vertex buffer objects (e.g.: vertex's position, normal, and color)
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    //Genereate a GPU buffer to store the positions of the vertices
    size_t vertexBufferSize = sizeof(float) * m_vertexPositions.size();
    glGenBuffers(1, &m_posVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, m_vertexPositions.data(), GL_DYNAMIC_READ);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    // normal
    size_t normalBufferSize = sizeof(float) * m_vertexNormals.size();
    glGenBuffers(1, &m_normalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_normalVbo);
    glBufferData(GL_ARRAY_BUFFER, normalBufferSize, m_vertexNormals.data(), GL_DYNAMIC_READ);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    //texture
    size_t textureBufferSize = sizeof(float) * m_vertexTexCoords.size();
    glGenBuffers(1, &m_texCoordVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_texCoordVbo);
    glBufferData(GL_ARRAY_BUFFER, textureBufferSize, m_vertexTexCoords.data(), GL_DYNAMIC_READ);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(2);

    //index
    size_t indexBufferSize = sizeof(unsigned int)*m_triangleIndices.size();
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, m_triangleIndices.data(), GL_DYNAMIC_READ);

    //deactivate the VAO, will be activated again when rendering
    glBindVertexArray(0);
}
std::shared_ptr<Mesh> Mesh::genSphere(const size_t resolution){
    auto mesh = std::make_shared<Mesh>();
    //generate vertex
    const float pi = 3.1415926;
    for (size_t latitude = 0; latitude<=resolution; latitude++){
        float theta = pi * latitude / resolution; // polar angle
        for (size_t longitude = 0; longitude<=resolution; longitude++){
            float phi = pi * longitude * 2 / resolution;
            float x = sin(theta) * cos(phi);
            float y = cos(theta);
            float z = - sin(theta) * sin(phi);
            mesh->m_vertexPositions.push_back(x);
            mesh->m_vertexPositions.push_back(y);
            mesh->m_vertexPositions.push_back(z);
            // generate normals
            mesh->m_vertexNormals.push_back(x);
            mesh->m_vertexNormals.push_back(y);
            mesh->m_vertexNormals.push_back(z);
            // generate vertexTexCoords
            float u = float(longitude) / resolution;
            float v = float(latitude) / resolution;
            mesh->m_vertexTexCoords.push_back(u);
            mesh->m_vertexTexCoords.push_back(v);
            // std::cout<<v<<std::endl;
        }
    }
    //generate triangle indices
    for(size_t latitude = 0; latitude<=resolution; latitude++){
        for(size_t longitude = 0; longitude<=resolution; longitude++){
          int tmp1 = latitude * resolution + longitude;
          int tmp2 = tmp1 + resolution;
          mesh->m_triangleIndices.push_back(tmp1);
          mesh->m_triangleIndices.push_back(tmp2);
          mesh->m_triangleIndices.push_back(tmp2 + 1);
          mesh->m_triangleIndices.push_back(tmp1);
          mesh->m_triangleIndices.push_back(tmp2 + 1);
          mesh->m_triangleIndices.push_back(tmp1 + 1);
        }
    }
    
    return mesh;
}
void Mesh::render(){
    glBindVertexArray(m_vao);
    const glm::vec3 camPosition = g_camera.getPosition();
    glUniform3f(glGetUniformLocation(g_program, "camPos"), camPosition[0], camPosition[1], camPosition[2]);
    glUniformMatrix4fv(glGetUniformLocation(g_program, "modelMat"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
    glUniform3fv(glGetUniformLocation(g_program, "lightDir"), 1, glm::value_ptr(-position + sunPosition)); 
    glDrawElements(GL_TRIANGLES, m_triangleIndices.size(), GL_UNSIGNED_INT, 0);
}



// Executed each time the window is resized. Adjust the aspect ratio and the rendering viewport to the current window.
void windowSizeCallback(GLFWwindow* window, int width, int height) {
  g_camera.setAspectRatio(static_cast<float>(width)/static_cast<float>(height));
  glViewport(0, 0, (GLint)width, (GLint)height); // Dimension of the rendering region in the window
}

// Executed each time a key is entered.
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS && key == GLFW_KEY_W) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else if(action == GLFW_PRESS && key == GLFW_KEY_F) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  } else if(action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)) {
    glfwSetWindowShouldClose(window, true); // Closes the application if the escape key is pressed
  }
}

void errorCallback(int error, const char *desc) {
  std::cout <<  "Error " << error << ": " << desc << std::endl;
}

void initGLFW() {
  glfwSetErrorCallback(errorCallback);

  // Initialize GLFW, the library responsible for window management
  if(!glfwInit()) {
    std::cerr << "ERROR: Failed to init GLFW" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // Before creating the window, set some option flags
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  // Create the window
  g_window = glfwCreateWindow(
    1024, 768,
    "Interactive 3D Applications (OpenGL) - Simple Solar System",
    nullptr, nullptr);
  if(!g_window) {
    std::cerr << "ERROR: Failed to open window" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  // Load the OpenGL context in the GLFW window using GLAD OpenGL wrangler
  glfwMakeContextCurrent(g_window);
  glfwSetWindowSizeCallback(g_window, windowSizeCallback);
  glfwSetScrollCallback(g_window, scroll_callback);
  glfwSetKeyCallback(g_window, keyCallback);
}

void initOpenGL() {
  // Load extensions for modern OpenGL
  if(!gladLoadGL(glfwGetProcAddress)) {
    std::cerr << "ERROR: Failed to initialize OpenGL context" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  glCullFace(GL_BACK); // Specifies the faces to cull (here the ones pointing away from the camera)
  glEnable(GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
  glDepthFunc(GL_LESS);   // Specify the depth test for the z-buffer
  glEnable(GL_DEPTH_TEST);      // Enable the z-buffer test in the rasterization
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // specify the background color, used any time the framebuffer is cleared
}

// Loads the content of an ASCII file in a standard C++ string
std::string file2String(const std::string &filename) {
  std::ifstream t(filename.c_str());
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

// Loads and compile a shader, before attaching it to a program
void loadShader(GLuint program, GLenum type, const std::string &shaderFilename) {
  GLuint shader = glCreateShader(type); // Create the shader, e.g., a vertex shader to be applied to every single vertex of a mesh
  std::string shaderSourceString = file2String(shaderFilename); // Loads the shader source from a file to a C++ string
  const GLchar *shaderSource = (const GLchar *)shaderSourceString.c_str(); // Interface the C++ string through a C pointer
  glShaderSource(shader, 1, &shaderSource, NULL); // load the vertex shader code
  glCompileShader(shader);
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR in compiling " << shaderFilename << "\n\t" << infoLog << std::endl;
  }
  glAttachShader(program, shader);
  glDeleteShader(shader);
}

void initGPUprogram() {
  g_program = glCreateProgram(); // Create a GPU program, i.e., two central shaders of the graphics pipeline
  loadShader(g_program, GL_VERTEX_SHADER, "./shader/Blinn-Phong/Blinn-PhongVertexShader.glsl");
  loadShader(g_program, GL_FRAGMENT_SHADER, "./shader/Blinn-Phong/Blinn-PhongFragmentShader.glsl");
  glLinkProgram(g_program); // The main GPU program is ready to be handle streams of polygons
  glUseProgram(g_program);

  backgroundTex = loadTextureFromFileToGPU("media/background.jpg");
  glUniform1i(glGetUniformLocation(g_program, "material.backgroundTex"), 10);
  sunTex = loadTextureFromFileToGPU("media/sun.jpg");
  glUniform1i(glGetUniformLocation(g_program, "material.sunTex"), 0);
  earthTex = loadTextureFromFileToGPU("media/2earth.jpg");
  glUniform1i(glGetUniformLocation(g_program, "material.earthTex"), 1);
  moonTex = loadTextureFromFileToGPU("media/moon.jpg");
  glUniform1i(glGetUniformLocation(g_program, "material.moonTex"), 2);
  venusTex = loadTextureFromFileToGPU("media/1venus_surface.jpg");
  glUniform1i(glGetUniformLocation(g_program, "material.venusTex"), 3);
  marsTex = loadTextureFromFileToGPU("media/3mars.jpg");
  glUniform1i(glGetUniformLocation(g_program, "material.marsTex"), 4);
  jupiterTex = loadTextureFromFileToGPU("media/4jupiter.jpg");
  glUniform1i(glGetUniformLocation(g_program, "material.jupiterTex"), 5);
  saturnTex = loadTextureFromFileToGPU("media/5saturn.jpg");
  glUniform1i(glGetUniformLocation(g_program, "material.saturnTex"), 6);
  uranusTex = loadTextureFromFileToGPU("media/6uranus.jpg");
  glUniform1i(glGetUniformLocation(g_program, "material.uranusTex"), 7);
  neptuneTex = loadTextureFromFileToGPU("media/7neptune.jpg");
  glUniform1i(glGetUniformLocation(g_program, "material.neptuneTex"), 8);
  mercuryTex = loadTextureFromFileToGPU("media/0mercury.jpg");
  glUniform1i(glGetUniformLocation(g_program, "material.mercuryTex"), 9);
}

// Define your mesh(es) in the CPU memory
void initCPUgeometry() {
  // TODO: add vertices and indices for your mesh(es)
  sunMesh = Mesh::genSphere(16);
  earthMesh = Mesh::genSphere(16);
  moonMesh = Mesh::genSphere(16);
  mercuryMesh = Mesh::genSphere(16);
  venusMesh = Mesh::genSphere(16);
  marsMesh = Mesh::genSphere(16);
  jupiterMesh = Mesh::genSphere(16);
  saturnMesh = Mesh::genSphere(16);
  uranusMesh = Mesh::genSphere(16);
  neptuneMesh = Mesh::genSphere(16);
}

void initGPUgeometry() {
  // Create a single handle, vertex array object that contains attributes,
  // vertex buffer objects (e.g., vertex's position, normal, and color)
  g_background.init();
  sunMesh->init();
  earthMesh->init();
  moonMesh->init();
  mercuryMesh->init();
  venusMesh->init();
  marsMesh->init();
  jupiterMesh->init();
  saturnMesh->init();
  uranusMesh->init();
  neptuneMesh->init();
  // glBindVertexArray(0); // deactivate the VAO for now, will be activated again when rendering
}

void initCamera() {
  int width, height;
  glfwGetWindowSize(g_window, &width, &height);
  g_camera.setAspectRatio(static_cast<float>(width)/static_cast<float>(height));
  
  g_camera.setPosition(glm::vec3(0.0, 20.0, 80.0));
  g_camera.setNear(0.1);
  g_camera.setFar(200.1);
}

void init() {
  initGLFW(); //create window
  initOpenGL(); //GLAD set background color
  initCPUgeometry(); // vertexes indices vertexTexCoords normal
  initGPUprogram(); //bind shader to GPU program
  initGPUgeometry(); // VAO, VBO, IBO
  initCamera(); // set camera position
}

void clear() {
  glDeleteProgram(g_program);

  glfwDestroyWindow(g_window);
  glfwTerminate();
}

// The main rendering call
void render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.

  const glm::mat4 viewMatrix = g_camera.computeViewMatrix();
  const glm::mat4 projMatrix = g_camera.computeProjectionMatrix();

  glUniformMatrix4fv(glGetUniformLocation(g_program, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMatrix)); // compute the view matrix of the camera and pass it to the GPU program
  glUniformMatrix4fv(glGetUniformLocation(g_program, "projMat"), 1, GL_FALSE, glm::value_ptr(projMatrix)); // compute the projection matrix of the camera and pass it to the GPU program
  //render background
  glUniform1i(glGetUniformLocation(g_program, "is_which"), -1);
  glActiveTexture(GL_TEXTURE10);
  glBindTexture(GL_TEXTURE_2D, backgroundTex);
  g_background.render();
  // render sun
  glm::mat4 sunModelMatrix = glm::translate(glm::mat4(1.0f), sunPosition);
  sunModelMatrix = glm::scale(sunModelMatrix, glm::vec3(kSizeSun)); // scale
  glUniform3fv(glGetUniformLocation(g_program, "lightColor"), 1, glm::value_ptr(sunColor)); 
  glUniform1i(glGetUniformLocation(g_program, "is_which"), 0);
  sunMesh->update_model(sunModelMatrix);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sunTex);
  sunMesh->render();  
  // render earth
  glUniform1i(glGetUniformLocation(g_program, "is_which"), 1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, earthTex);
  earthMesh->render();  
  // render moon
  glUniform1i(glGetUniformLocation(g_program, "is_which"), 2);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, moonTex);
  moonMesh->render();  
  // render mercury
  glUniform1i(glGetUniformLocation(g_program, "is_which"), 3);
  glActiveTexture(GL_TEXTURE9);
  glBindTexture(GL_TEXTURE_2D, mercuryTex);
  mercuryMesh->render();  
  // render venus
  glUniform1i(glGetUniformLocation(g_program, "is_which"), 4);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, venusTex);
  venusMesh->render();  
  // render mars
  glUniform1i(glGetUniformLocation(g_program, "is_which"), 5);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, marsTex);
  marsMesh->render();  
  // render jupiter
  glUniform1i(glGetUniformLocation(g_program, "is_which"), 6);
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, jupiterTex);
  jupiterMesh->render();  
  // render saturn
  glUniform1i(glGetUniformLocation(g_program, "is_which"), 7);
  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, saturnTex);
  saturnMesh->render();  
  // render uranus
  glUniform1i(glGetUniformLocation(g_program, "is_which"), 8);
  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_2D, uranusTex);
  uranusMesh->render();  
  // render neptune
  glUniform1i(glGetUniformLocation(g_program, "is_which"), 9);
  glActiveTexture(GL_TEXTURE8);
  glBindTexture(GL_TEXTURE_2D, neptuneTex);
  neptuneMesh->render();  
}
struct XZAngle {
    float RotationAngle, X, Z, kSize;
};
XZAngle cal_X_Z(float currentTimeInSec, float kRotatePeriod,float kOrbitPeriod,float kRadOrbit, float kSize){
  float pi = 3.1415926;
  float RotationAngle = fmod(currentTimeInSec, kRotatePeriod) / kRotatePeriod * 2 * pi;
  float OrbitAngle = fmod(currentTimeInSec, kOrbitPeriod) / kOrbitPeriod * 2 * pi;
  float X = sin(OrbitAngle) * kRadOrbit;
  float Z = cos(OrbitAngle) * kRadOrbit;
  return {RotationAngle, X, Z, kSize};
}
glm::mat4 calMatrix(XZAngle xza){
  float X = xza.X;
  float Z = xza.Z;
  float RotationAngle = xza.RotationAngle;
  float kSize = xza.kSize;
  glm::mat4 ModelMatrix = glm::mat4(1.0f);
  ModelMatrix = glm::translate(ModelMatrix, glm::vec3(X, 0.0f, Z)); //orbition
  ModelMatrix = glm::rotate(ModelMatrix, RotationAngle, glm::vec3(0.0f, 1.0f, 0.0f)); //rotation: y axis
  ModelMatrix = glm::scale(ModelMatrix, glm::vec3(kSize)); //scale
  return ModelMatrix;
}
// Update any accessible variable based on the current time
void update(const float currentTimeInSec) {
  float pi = 3.1415926;
  float earthRotationAngle = fmod(currentTimeInSec, kRotatePeriodEarth) / kRotatePeriodEarth * 2 * pi;
  float earthOrbitAngle = fmod(currentTimeInSec, kOrbitPeriodEarth) / kOrbitPeriodEarth * 2 * pi;
  float moonRotationAngle = fmod(currentTimeInSec, kRotatePeriodMoon) / kRotatePeriodMoon * 2 * pi;
  float moonOrbitAngle = fmod(currentTimeInSec, kOrbitPeriodMoon) / kOrbitPeriodMoon * 2 * pi;
  float earthX = sin(earthOrbitAngle) * (kRadOrbitEarth+ kSizeSun/2);
  float earthZ = cos(earthOrbitAngle) * (kRadOrbitEarth+ kSizeSun/2);
  float relmoonX = sin(moonOrbitAngle) * (kRadOrbitMoon + kSizeEarth/2);
  float relmoonZ = cos(moonOrbitAngle) * (kRadOrbitMoon + kSizeEarth/2);
  XZAngle mercuryXZA = cal_X_Z(currentTimeInSec, kRotatePeriodMercury, kOrbitPeriodMercury, kRadOrbitMercury, kSizeMercury);
  glm::mat4 mercuryModelMatrix = calMatrix(mercuryXZA);
  XZAngle venusXZA = cal_X_Z(currentTimeInSec, kRotatePeriodVenus, kOrbitPeriodVenus, kRadOrbitVenus, kSizeVenus);
  glm::mat4 venusModelMatrix = calMatrix(venusXZA);
  XZAngle marsXZA = cal_X_Z(currentTimeInSec, kRotatePeriodMars, kOrbitPeriodMars, kRadOrbitMars, kSizeMars);
  glm::mat4 marsModelMatrix = calMatrix(marsXZA);
  XZAngle jupiterXZA = cal_X_Z(currentTimeInSec, kRotatePeriodJupiter, kOrbitPeriodJupiter, kRadOrbitJupiter, kSizeJupiter);
  glm::mat4 jupiterModelMatrix = calMatrix(jupiterXZA);
  XZAngle saturnXZA = cal_X_Z(currentTimeInSec, kRotatePeriodSaturn, kOrbitPeriodSaturn, kRadOrbitSaturn, kSizeSaturn);
  glm::mat4 saturnModelMatrix = calMatrix(saturnXZA);
  XZAngle uranusXZA = cal_X_Z(currentTimeInSec, kRotatePeriodUranus, kOrbitPeriodUranus, kRadOrbitUranus, kSizeUranus);
  glm::mat4 uranusModelMatrix = calMatrix(uranusXZA);
  XZAngle neptuneXZA = cal_X_Z(currentTimeInSec, kRotatePeriodNeptune, kOrbitPeriodNeptune, kRadOrbitNeptune, kSizeNeptune);
  glm::mat4 neptuneModelMatrix = calMatrix(neptuneXZA);

  glm::mat4 earthModelMatrix = glm::mat4(1.0f);
  earthModelMatrix = glm::translate(earthModelMatrix, glm::vec3(earthX, 0.0f, earthZ)); //orbition
  earthModelMatrix = glm::rotate(earthModelMatrix, glm::radians(23.5f), glm::vec3(0.0f, 0.0f, 1.0f)); //tilt: x axis // undo
  earthModelMatrix = glm::rotate(earthModelMatrix, earthRotationAngle, glm::vec3(0.0f, 1.0f, 0.0f)); //rotation: y axis
  earthModelMatrix = glm::scale(earthModelMatrix, glm::vec3(kSizeEarth)); //scale
  earthMesh->update_model(earthModelMatrix);
  earthMesh->update_position(glm::vec3(earthX, 0.0, earthZ));
  
  glm::mat4 moonModelMatrix = glm::mat4(1.0f);
  moonModelMatrix = glm::translate(moonModelMatrix, glm::vec3(earthX, 0.0f, earthZ));
  moonModelMatrix = glm::translate(moonModelMatrix, glm::vec3(relmoonX, 0.0, relmoonZ));
  moonModelMatrix = glm::rotate(moonModelMatrix, moonRotationAngle, glm::vec3(0.0, 1.0, 0.0));
  moonModelMatrix = glm::scale(moonModelMatrix, glm::vec3(kSizeMoon));
  moonMesh->update_model(moonModelMatrix);
  moonMesh->update_position(glm::vec3(earthX+relmoonX, 0.0, earthZ+relmoonZ));

  mercuryMesh->update_model(mercuryModelMatrix);
  mercuryMesh->update_position(glm::vec3(mercuryXZA.X, 0.0, mercuryXZA.Z));
  venusMesh->update_model(venusModelMatrix);
  venusMesh->update_position(glm::vec3(venusXZA.X, 0.0, venusXZA.Z));
  marsMesh->update_model(marsModelMatrix);
  marsMesh->update_position(glm::vec3(marsXZA.X, 0.0, marsXZA.Z));
  jupiterMesh->update_model(jupiterModelMatrix);
  jupiterMesh->update_position(glm::vec3(jupiterXZA.X, 0.0, jupiterXZA.Z));
  saturnMesh->update_model(saturnModelMatrix);
  saturnMesh->update_position(glm::vec3(saturnXZA.X, 0.0, saturnXZA.Z));
  uranusMesh->update_model(uranusModelMatrix);
  uranusMesh->update_position(glm::vec3(uranusXZA.X, 0.0, uranusXZA.Z));
  neptuneMesh->update_model(neptuneModelMatrix);
  neptuneMesh->update_position(glm::vec3(neptuneXZA.X, 0.0, neptuneXZA.Z));

}

int main(int argc, char ** argv) {
  init(); // Your initialization code (user interface, OpenGL states, scene with geometry, material, lights, etc)

  while(!glfwWindowShouldClose(g_window)) {
    update(static_cast<float>(glfwGetTime()));
    
    render();
    glfwSwapBuffers(g_window);
    glfwPollEvents();
  }
  clear();
  return EXIT_SUCCESS;
}
